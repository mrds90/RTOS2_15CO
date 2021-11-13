/*=============================================================================
 * Copyright (c) 2020, Martin N. Menendez <menendezmartin81@gmail.com>
 * All rights reserved.
 * License: Free
 * Date: 2020/07/26
 * Version: v1.1
 *===========================================================================*/

/*=====[Inclusion of own header]=============================================*/

#include "app.h"

/*=====[Inclusions of private function dependencies]=========================*/

/*=====[Definition macros of private constants]==============================*/

/*=====[Private function-like macros]========================================*/

/*=====[Definitions of private data types]===================================*/

/*=====[Definitions of external public global variables]=====================*/

/*=====[Definitions of public global variables]==============================*/

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

//app_task recibe como parametro los datos de una UART con el driver inicializado
//se queda esperando a que llegue un paquete, lo procesa y lo envia por la UART
void app_task( void* pvParameters )
{
    app_t *appinstance = pvParameters;
    char *TXbuffer;


    while ( TRUE )
    {
        gpioToggle( CHECK_LED );

        TXbuffer = protocol_wait_frame( appinstance->protocol_hnd ); //espero un paquete

        if ( TXbuffer!=NULL )	//si recibo null es porque ocurrio un error en la comunicacion
        {
            Check_packet( TXbuffer ); //Analizo si el paquete es valido y luego lo paso a mayuscula si corresponde o cargo un mensaje de error
            protocol_send( appinstance->protocol_hnd, TXbuffer );   //envio el paquete procesado o el mensaje de error.
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

        BaseType_t res = xTaskCreate(
                               app_task,                     // Funcion de la tarea a ejecutar
                               ( const char * )"tarea de chequeo de paquetes",   // Nombre de la tarea como String amigable para el usuario
                               configMINIMAL_STACK_SIZE*2, // Cantidad de stack de la tarea
                               ( void* ) app_instance,                        // Parametros de tarea
                               tskIDLE_PRIORITY+2,         // Prioridad de la tarea
                               &( app_instance->app_task_hnd )                  // Puntero a la tarea creada en el sistema
                           );

        return (!res)?(FALSE):(TRUE);

    }
    else
    {	 // La instancia es null
        return FALSE;
    }
}


/*=====[Implementations of interrupt functions]==============================*/

/*=====[Implementations of private functions]================================*/

