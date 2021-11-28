/*=============================================================================
 * Copyright (c) 2021, Franco Bucafusco <franco_bucafusco@yahoo.com.ar>
 * 					   Martin N. Menendez <mmenendez@fi.uba.ar>
 * All rights reserved.
 * License: Free
 * Date: 2021/05/03
 * Version: v1.3
 *===========================================================================*/
/*=====[Inclusion of own header]=============================================*/

#include "protocol.h"

/*=====[Inclusions of private function dependencies]=========================*/

/*=====[Definition macros of private constants]==============================*/

/*=====[Private function-like macros]========================================*/

/*=====[Definitions of private data types]===================================*/

/*=====[Definitions of external public global variables]=====================*/

/*=====[Definitions of public global variables]==============================*/

/*=====[Definitions of private global variables]=============================*/

/*=====[Prototypes (declarations) of private functions]======================*/

void protocol_onTxCallback( void* );
void protocol_onRxCallback( void * );
bool_t protocol_txInterruptEnable( protocol_t* );
bool_t protocol_rxInterruptEnable( protocol_t* );
void protocol_onTxTimeOutCallback( TimerHandle_t );
void protocol_onRxTimeOutCallback( TimerHandle_t );

/*=====[Implementations of public functions]=================================*/

// Inicializacion de parte de transmision del driver
// Requiere que se le pase una variable del tipo driverConfig_t
// con los siguientes datos:
// 	selected_protocol.uartValue = (por ej:UART_USB);
//	selected_protocol.baudRate = (por ej: 115200);
bool_t protocol_init( protocol_t* selected_protocol )
{
    // Inicializamos la UART que se ha seleccionado y con el baud rate especificado.
    uartConfig( selected_protocol->uartValue, selected_protocol->baudRate );

    // Creamos el timer que cumplira la funcion de time out y poder separar los paquetes.

    selected_protocol->onTxTimeOut = xTimerCreate( "TX Time Out", PROTOCOL_TIMEOUT,pdFALSE, ( void* ) selected_protocol, protocol_onTxTimeOutCallback );
    selected_protocol->onRxTimeOut = xTimerCreate( "RX Time Out", PROTOCOL_TIMEOUT,pdFALSE, ( void* ) selected_protocol, protocol_onRxTimeOutCallback );

    // Creamos la cola para se�alizar la recepcion de un dato valido hacia la aplicacion.
    selected_protocol->onRxQueue = xQueueCreate( POOL_TOTAL_BLOCKS, sizeof( char* ) );

    //	Creo una cola donde van a ir los bloque que tengo que mandar por UART
    selected_protocol->onTxQueue = xQueueCreate( POOL_TOTAL_BLOCKS, sizeof( char* ) ); //La cola va a tener tantos elementos como bloques de memoria pueda tener el pool

    //	Reservo memoria para el memory pool
    selected_protocol->Pool_pointer = ( char* ) pvPortMalloc( POOL_SIZE * sizeof( char ) );

    //	Creo el pool de memoria que va a usarse para la transmision
    QMPool_init( &selected_protocol->Pool, ( void* ) selected_protocol->Pool_pointer,POOL_SIZE * sizeof( char ),BLOCK_SIZE );
    //         (-------- Pool --------,------ Memoria para el pool ------,---- Cant. de memoria ----,--N Bloques--);

    //	Validamos si todos los recursos que requeran de memoria fueron creados exitosamente.
    if ( ( selected_protocol->onRxTimeOut == NULL ) ||		// timer de RX
            ( selected_protocol->onTxTimeOut == NULL ) ||	// timer de TX
            ( selected_protocol->onRxQueue == NULL ) ||      // cola para RX
            ( selected_protocol->onTxQueue == NULL ) ||      // cola para TX
            ( selected_protocol->Pool_pointer == NULL ) )       // Memoria pedida para el pool
    {
        // No hay memoria suficiente. Por este motivo, retornamos false.
        return( FALSE );
    }


    selected_protocol->txCounter = 0; // Inicializo el contador de bytes enviados
    selected_protocol->rxLen = 0;	 // Inicializo el contador de tama�o RX
    selected_protocol->txLen = 0;	 // Inicializo el contador de tama�o TX

    // Asigno un bloque de memoria para la primera recepcion de la UART
    selected_protocol->rxBlock = ( char* ) QMPool_get( &selected_protocol->Pool, 0 );  //pido un bloque del pool

    // Configuramos el callback del evento de recepción y habilitamos su interrupción.
    if( ( protocol_rxInterruptEnable( selected_protocol ) ) )
    {}
    else  //se eligio una UART no valida
    {
        return ( FALSE );
    }

    // Habilitamos todas las interrupciones de la UART seleccionada.
    uartInterrupt( selected_protocol->uartValue, TRUE );

    // Si todos los elementos fueron creados e inicializados correctamente, indicamos que el driver está listo para ser utilizado.
    return( TRUE );
}

//	protocol_send(driverConfig_t* selected_protocol, char *txBlock) transmite el bloque
//	txBlock por la UART indicada por selected_protocol
void protocol_send( protocol_t* selected_protocol, char *txBlock )
{
    xQueueSend( selected_protocol->onTxQueue, &txBlock, portMAX_DELAY ); //Envio a la cola de transmision el blocke a transmitir
    taskENTER_CRITICAL();  //no permito que se modifique txcounter
    if ( selected_protocol->txCounter == 0 ) //si se esta enviando algo no llamo a la interrupcion para no interrumpir el delay
    {
        protocol_txInterruptEnable( selected_protocol );
        uartSetPendingInterrupt( selected_protocol->uartValue );
    }
    taskEXIT_CRITICAL();

}

/**
   @brief    se queda esperando un paquete de la UART seleccionada y devuelve un puntero al bloque de memoria
 */
char* protocol_wait_frame( protocol_t* selected_protocol ) //  == get(&objeto)
{
    char*dato_rx;
    xQueueReceive( selected_protocol->onRxQueue,( void * )&dato_rx,portMAX_DELAY );   //espero a que venga un bloque por la cola
    return dato_rx;
}

/*=====[Implementations of private functions]================================*/

//	protocol_onTxTimeOutCallback
// Timer para la separacion de paquetes

void protocol_onTxTimeOutCallback( TimerHandle_t xTimer )
{
    BaseType_t xHigherPriorityTaskWoken;
    xHigherPriorityTaskWoken = pdFALSE;

    // Obtenemos los datos de la UART seleccionada, aprovechando el campo reservado para el Timer ID.
    protocol_t* selected_protocol = ( protocol_t* ) pvTimerGetTimerID( xTimer );

    //Inicio seccion critica
    taskENTER_CRITICAL();

    uartTxWrite( selected_protocol->uartValue,'\r' );
    //uartTxWrite( selected_protocol->uartValue ,'\n' );

    // Libero el bloque de memoria que ya fue trasmitido
    QMPool_put( &selected_protocol->Pool, selected_protocol->txBlock );

    selected_protocol->txCounter = 0; //Reinicio el contador de bytes transmitidos para la siguiente transmision

    taskEXIT_CRITICAL(); // Fin de seccion critica

    // Si hay mensajes esperando, vuelvo a prender la interrupcion y la disparo
    if ( uxQueueMessagesWaiting ( selected_protocol->onTxQueue ) )
    {
        protocol_txInterruptEnable ( selected_protocol );
        uartSetPendingInterrupt ( selected_protocol->uartValue );
    }
    else
    {
        /* No es obligatorio deshabilitar interrupciones , se  vuelve halfduplex */

        // Volvemos a encender la interrupcion de RX      //
        protocol_rxInterruptEnable ( selected_protocol );
        uartSetPendingInterrupt ( selected_protocol->uartValue );
    }
}

// Callback para el timeout definido en PROTOCOL_TIMEOUT.
void protocol_onRxTimeOutCallback( TimerHandle_t xTimer )
{
    int crcTemp = 0;
    char* aux = NULL;

    // Obtenemos los datos de la UART, aprovechando el campo reservado para el Timer ID.
    protocol_t* selected_protocol = ( protocol_t* ) pvTimerGetTimerID( xTimer );

    // Inicio seccion critica
    taskENTER_CRITICAL();

    if ( selected_protocol->rxLen <= PACKET_SIZE ) //si la cantidad de bytes es correcta
    {
        selected_protocol->rxBlock[0] = selected_protocol->rxLen;
        crcTemp = str_to_checksum( selected_protocol->rxBlock );

        // Si el CRC es cero, significa que el paquete es válido.
        if ( crcTemp == ( selected_protocol->rxBlock[selected_protocol->rxLen-1]-'0' ) )
        {
        	aux =  selected_protocol->rxBlock;

            //Una vez que el bloque esta en la cola pido otro bloque para el siguiente paquete. Es responsabilidad de
            //la aplicacion liberar el bloque mediante una transmision o con la funcion putBuffer()
            selected_protocol->rxBlock = ( char* ) QMPool_get( &selected_protocol->Pool,0 ); //pido un bloque del pool

            //Chequeo si tengo un bloque de memoria, sino anulo la recepcion de paquetes
            if ( selected_protocol->rxBlock == NULL )
            {
                uartCallbackClr( selected_protocol->uartValue, UART_RECEIVE );
            }
        }
        else
        {
            selected_protocol->rxLen = 0;    //pongo el contador n cero y me quedo con el bloque de memoria
            // Aca podriamos poner alguna cosa para avisar el error de checksum
        }

        // Caso contrario, el paquete es inválido y se debe descartar (no necesito hacer nada)
    }
    else
    {
        selected_protocol->rxLen = 0;    //pongo el contador n cero y me quedo con el bloque de memoria
    }

    // Reiniciamos el índice para cuando llegue un nuevo paquete.
    selected_protocol->rxLen = 0;
    crcTemp = 0; //Borro el acumulador de CRC para el siguiente paquete

    taskEXIT_CRITICAL(); // Salgo de la zona critica

    if (aux != NULL)
    {
    	// Se escribe en la cola el buffer completo y a su vez avisamos a la aplicación de que un paquete correcto se debe procesar.
         xQueueSend( selected_protocol->onRxQueue,( void* )&( aux ), portMAX_DELAY );
    }

}

//rxInterruptEnable(driverConfig_t* selected_protocol) Habilita la interrupcion de la UART para recepcion
//indicada por selected_protocol
bool_t protocol_rxInterruptEnable( protocol_t* selected_protocol )
{
    switch ( selected_protocol->uartValue )
    {
        case UART_GPIO:
        case UART_USB:
        case UART_232:
        {
            uartCallbackSet( selected_protocol->uartValue, UART_RECEIVE, protocol_onRxCallback,( void* ) selected_protocol );
            break;
        }
        default:
        {
            // No se ha seleccionado una UART correcta, por lo que retornamos false como un error.
            return ( FALSE );
        }
    }

    return ( TRUE );
}

//txInterruptEnable(driverConfig_t* selected_protocol) Habilita la interrupcion de la UART para transmision
//indicada por selected_protocol
bool_t protocol_txInterruptEnable( protocol_t* selected_protocol )
{
    switch ( selected_protocol->uartValue )
    {
        case UART_GPIO:
        case UART_USB:
        case UART_232:
        {
            uartCallbackSet( selected_protocol->uartValue, UART_TRANSMITER_FREE,protocol_onTxCallback, ( void* ) selected_protocol );
            break;
        }
        default:
        {
            // No se ha seleccionado una UART correcta, por lo que retornamos false como un error.
            return ( FALSE );
        }
    }

    return ( TRUE );
}

/*=====[Implementations of interrupt functions]==============================*/

// Callback para la recepción
void protocol_onRxCallback( void *param )
{
    UBaseType_t uxSavedInterruptStatus;
    BaseType_t xTaskWokenByReceive= pdFALSE; //Variable para evaluar el cambio de contexto al finaliza la interrupcion

    //Elijo los datos correspondientes al Callback
    protocol_t* selected_protocol = param;
    uxSavedInterruptStatus = taskENTER_CRITICAL_FROM_ISR();	// Protejo selected_protocol
    //si llegan mas bytes de lo que entra en el buffer no los leo. Luego descarto el paquete en el timer de timeout
    if ( selected_protocol->rxLen < PACKET_SIZE )
    {
        // Obtenemos el byte de la UART seleccionada y reservo el primer byte para guardar la longitud del string
        selected_protocol->rxBlock[selected_protocol->rxLen + 1] = uartRxRead( selected_protocol->uartValue );
        //selected_protocol->rxCrc = crc8_calc( selected_protocol->rxCrc,&selected_protocol->rxBlock[selected_protocol->rxLen + 1], 1 ); //acumulo el crc en cada byte que recibo
    }
    else
    {
        selected_protocol->rxLen = PACKET_SIZE + 1; //limito el valor para no desbordar la variable
    }



    // Incrementamos el valor del índice.
    selected_protocol->rxLen++;
    taskEXIT_CRITICAL_FROM_ISR( uxSavedInterruptStatus );

    // Inicializamos el timer.
      xTimerStartFromISR( selected_protocol->onRxTimeOut, &xTaskWokenByReceive );

    //Hago la evaluacion de cambio de cambio de contexto necesario.
    if ( xTaskWokenByReceive != pdFALSE )
    {
        taskYIELD ();
    }

}


//	uartSendDataCRC transmite los bloques que llegan por una cola y los envia por la UART seleccionada
//	Todas las callback de interrupcion de UART llaman a esta función para hacer la transmisión.
void protocol_onTxCallback( void*param )
{
    protocol_t *selected_protocol = ( protocol_t * ) param;
    BaseType_t xTaskWokenByReceive = pdFALSE; //Variable para evaluar el cambio de contexto al finaliza la interrupcion
    UBaseType_t uxSavedInterruptStatus;

    //Armo una seccion crtitica para proteger las variables de txCounter
    uxSavedInterruptStatus = taskENTER_CRITICAL_FROM_ISR();

    //	Si el contador esta en cero tengo que tomar un bloque de la cola, calcular CRC
    if ( selected_protocol->txCounter == 0 )
    {
        //	Tomo el bloque a transmitir de la cola
        xQueueReceiveFromISR( selected_protocol->onTxQueue,( void * ) &selected_protocol->txBlock, &xTaskWokenByReceive );

        selected_protocol->txLen = selected_protocol->txBlock[0]+1; //en el primer byte del bloque viene la longitud del packete a enviar menos el CRC

        //	Calculo el CRC y lo pongo al final
        //selected_protocol->txBlock[selected_protocol->txLen + 1] = crc8_calc( 0,&selected_protocol->txBlock[1], selected_protocol->txLen );
        //selected_protocol->txLen += 2; //Incremento en 2 txLen: uno por el byte extra al comienzon y otro para que el CRC se transmita
        selected_protocol->txCounter++; //Incremento uno para no transmitir el primer byte del bloque

    }

    //	Si no llegué al final de la cadena transmito el dato
    if ( selected_protocol->txCounter < selected_protocol->txLen )
    {
        uartTxWrite( selected_protocol->uartValue,selected_protocol->txBlock[selected_protocol->txCounter] );
        selected_protocol->txCounter++;
    }

    //	Cuando termino de transmitir todos los bytes tengo que
    //	deshabilitar la interrupción e iniciar el timeout.
    if ( selected_protocol->txCounter == selected_protocol->txLen )
    {
        uartCallbackClr( selected_protocol->uartValue, UART_TRANSMITER_FREE ); //Deshabilito la interrupcion de TX
        xTimerStartFromISR( selected_protocol->onTxTimeOut, &xTaskWokenByReceive ); //Inicio el timer de Timeout
    }

    //Cierro la seccion critica
    taskEXIT_CRITICAL_FROM_ISR( uxSavedInterruptStatus );

    //Hago la evaluacion de cambio de cambio de contexto necesario.
    if ( xTaskWokenByReceive != pdFALSE )
    {
        taskYIELD ();
    }
}
