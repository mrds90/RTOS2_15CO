/*=============================================================================
 * Copyright (c) 2021, Franco Bucafusco <franco_bucafusco@yahoo.com.ar>
 * 					   Martin N. Menendez <mmenendez@fi.uba.ar>
 * All rights reserved.
 * License: Free
 * Date: 2021/05/03
 * Version: v1.3
 *===========================================================================*/
#include "FreeRTOS.h"
#include "task.h"
#include "sapi.h"

#include "app.h"
#include "protocol.h"
#include "FreeRTOSConfig.h"

#define UART UART_USB
#define BAUDRATE 115200
#define ERROR_LED LEDR
#define DRIVER_ERROR_MSG "Error al inicializar el driver.\r\n"
#define TASK_ERROR_MSG "Error al crear las tareas.\r\n"

protocol_t selected_protocol =
{
    .uartValue     = UART,
    .baudRate = BAUDRATE
}; //Instanciacion del driver que va a usar la tarea principal

app_t myapp;

int main( void )
{
    /* Inicializar la placa */

    boardConfig();

    //Inicializacion del driver de UART
    if ( protocol_init( &selected_protocol ) == FALSE )
    {
        gpioWrite( ERROR_LED,TRUE );  //Error al inicializar el driver
        printf( DRIVER_ERROR_MSG );
        while( TRUE );
    }

    BaseType_t res = xTaskCreate(
                           tickTask,                   	// Funcion de la tarea a ejecutar
                           ( const char * )"tickTask",	// Nombre de la tarea como String amigable para el usuario
                           configMINIMAL_STACK_SIZE*2, 	// Cantidad de stack de la tarea
                           0,                          	// Parametros de tarea
                           tskIDLE_PRIORITY+1,         	// Prioridad de la tarea
                           0                           	// Puntero a la tarea creada en el sistema
                       );

    configASSERT( res == pdPASS );

    bool_t state = app_create( &myapp, &selected_protocol );

    // Gestion de errores
    configASSERT( state );

    // Iniciar scheduler
    vTaskStartScheduler(); // Enciende tick | Crea idle y pone en ready | Evalua las tareas creadas | Prioridad mas alta pasa a running

    while( TRUE ); // Nunca debe llegar aca!

    return TRUE;
}
