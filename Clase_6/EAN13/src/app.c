/*=============================================================================
 * Copyright (c) 2021, Franco Bucafusco <franco_bucafusco@yahoo.com.ar>
 * 					   Martin N. Menendez <mmenendez@fi.uba.ar>
 * All rights reserved.
 * License: Free
 * Date: 2021/05/03
 * Version: v1.3
 *===========================================================================*/

/*=====[Inclusion of own header]=============================================*/

#include "app.h"
#include "AO.h"
#include "app_support.h"

/*=====[Inclusions of private function dependencies]=========================*/

/*=====[Definition macros of private constants]==============================*/

#define EVENT 			'2'

/*=====[Private function-like macros]========================================*/

/*=====[Definitions of private data types]===================================*/

/*=====[Definitions of external public global variables]=====================*/

/*=====[Definitions of public global variables]==============================*/
QueueHandle_t response_queue;
/*=====[Definitions of private global variables]=============================*/

/*=====[Prototypes (declarations) of private functions]======================*/

/*=====[Implementations of public functions]=================================*/

void tickTask( void* pvParameters )
{
    while( TRUE )
    {
        // Una tarea muy bloqueante para demostrar que la interrupcion funcina
        gpioToggle( TICK_LED );
        vTaskDelay( TASK_DELAY );
    }
}

void app_task_tx( void* pvParameters )
{
    app_t *appinstance = pvParameters;

    char* mensaje_respuesta;

    while( TRUE )
    {
        gpioToggle( LED3 );

        if( xQueueReceive( response_queue, &mensaje_respuesta, portMAX_DELAY ) )
        {
            protocol_send( appinstance->protocol_hnd, mensaje_respuesta );
        }
    }
}

//app_task recibe como parametro los datos de una UART con el driver inicializado
//se queda esperando a que llegue un paquete, lo procesa y lo envia por la UART
void app_task_rx( void* pvParameters )
{
    app_t *appinstance = pvParameters;
    char *TXbuffer;

    char evento;

	activeObject_t normalAO;
	normalAO.itIsAlive = FALSE;

	activeObject_t sietizadorAO;
	sietizadorAO.itIsAlive = FALSE;

    while ( TRUE )
    {
        gpioToggle( CHECK_LED );

        TXbuffer = protocol_wait_frame( appinstance->protocol_hnd ); //espero un paquete

        if ( TXbuffer!=NULL )	//si recibo null es porque ocurrio un error en la comunicacion
        {
        	evento = getCommand( TXbuffer );
        	if( evento != EVENT )
			{
				if( normalAO.itIsAlive == FALSE )
				{
					// Se crea el objeto activo, con el comando correspondiente y tarea asociada.
					activeObjectOperationCreate( &normalAO, Check_packet, activeObjectTask , response_queue);
				}

				// Y enviamos el dato a la cola para procesar.
				activeObjectEnqueue( &normalAO, &TXbuffer );
			}
			if( evento == EVENT )
			{
				if( sietizadorAO.itIsAlive == FALSE )
				{
					// Se crea el objeto activo, con el comando correspondiente y tarea asociada.
					activeObjectOperationCreate( &sietizadorAO, Sietizador, activeObjectTask , response_queue );
				}

				// Y enviamos el dato a la cola para procesar.
				activeObjectEnqueue( &sietizadorAO, &TXbuffer );
			}


        }
    }
}

/**
   @brief crea la instancia de la app

   @param app_instance
   @return true     se pudo
   @return false    no se pudo
 */
bool_t app_create( app_t* app_instance, void* protocol )
{
    if( app_instance != NULL )
    {
        app_instance->protocol_hnd  = protocol;
        BaseType_t res;

        res = xTaskCreate(app_task_rx, ( const char * )"tarea app_task_rx", configMINIMAL_STACK_SIZE*2, ( void* ) app_instance, tskIDLE_PRIORITY+2,&( app_instance->app_task_hnd ));
        configASSERT( res == pdPASS );
        res = xTaskCreate(app_task_tx, ( const char * )"tarea app_task_tx", configMINIMAL_STACK_SIZE*2, ( void* ) app_instance, tskIDLE_PRIORITY+2,&( app_instance->app_task_hnd ));
        configASSERT( res == pdPASS );

        response_queue = xQueueCreate( N_QUEUE_AO, sizeof( char* ) );
        configASSERT( response_queue != NULL );

        return (!res)?(FALSE):(TRUE);

    }
    else
    {	 // La instancia es null
        return FALSE;
    }
}


/*=====[Implementations of interrupt functions]==============================*/

/*=====[Implementations of private functions]================================*/

