/*=============================================================================
 * Copyright (c) 2021, Franco Bucafusco <franco_bucafusco@yahoo.com.ar>
 * 					   Martin N. Menendez <mmenendez@fi.uba.ar>
 * All rights reserved.
 * License: read LICENCE file
 * Date: 2021/10/03
 * Version: v1.2
 *===========================================================================*/

/*==================[inclusiones]============================================*/
#include <stdio.h>
#include <string.h>
#include "main.h"
#include "keys.h"
#include "user_tasks.h"
#include "object.h"

#include "qmpool.h"

/*==================[definiciones de datos internos]=========================*/
const gpioMap_t leds_t[] = {LED2,LED3};
const gpioMap_t gpio_t[] = {GPIO3,GPIO1};
tObjeto obj_1;
tObjeto obj_2;

/*==================[definiciones de datos externos]=========================*/
extern QMPool Pool_memoria;

/*==================[definiciones de funciones internas]=====================*/
void gpio_init( void )
{
    gpioInit( GPIO7, GPIO_OUTPUT );
    gpioInit( GPIO5, GPIO_OUTPUT );
    gpioInit( GPIO3, GPIO_OUTPUT );
    gpioInit( GPIO1, GPIO_OUTPUT );
}
/*==================[definiciones de funciones externas]=====================*/

// Implementacion de funcion de la tarea

void tarea_A( void* taskParmPtr )
{
    // ---------- CONFIGURACIONES ------------------------------
    TickType_t xPeriodicity =  MAX_RATE;
    TickType_t xLastWakeTime = xTaskGetTickCount();
    // ---------- REPETIR POR SIEMPRE --------------------------

    tMensaje mensaje;

    while( TRUE )
    {
        gpioWrite( LEDB, ON );
        vTaskDelay( MAX_RATE / 2 );


        mensaje = ( tMensaje ) QMPool_get( &Pool_memoria, 0 ); //pido un bloque del pool
        configASSERT( mensaje != NULL );			//<-- Gestion de errores

        memcpy( mensaje, MSG_LED, sizeof( MSG_LED ) );

        post( &obj_1, mensaje );

        gpioWrite( LEDB, OFF );

        vTaskDelayUntil( &xLastWakeTime, xPeriodicity );
    }
}

void tarea_C( void* taskParmPtr )
{
    // ---------- CONFIGURACIONES ------------------------------


    // ---------- REPETIR POR SIEMPRE --------------------------

    tMensaje mensaje;

    while( TRUE )
    {
        get( &obj_1, &mensaje );

        uartWriteString( USED_UART, mensaje );
        uartWriteString( USED_UART, "\r\n" );

        /* no hago free! el mensage se vuelve a mandar... */
        post( &obj_2,mensaje );


    }
}

void tarea_D( void* taskParmPtr )
{
    // ---------- CONFIGURACIONES ------------------------------



    tMensaje mensaje;

    // ---------- REPETIR POR SIEMPRE --------------------------

    while( TRUE )
    {
        get( &obj_2, &mensaje );

        // Libero el bloque de memoria que ya fue trasmitido
        QMPool_put( &Pool_memoria, mensaje );

        mensaje = NULL;
    }
}


void user_keys_event_handler_button_release( t_key_isr_signal* event_data )
{
    /* aloco localmente un puntero  */
    tMensaje mensaje;

    mensaje = ( tMensaje ) QMPool_get( &Pool_memoria, 0 ); //pido un bloque del pool

    configASSERT( mensaje != NULL );			//<-- Gestion de errores

    /* imprimo el mensaje */
    TickType_t dif = keys_get_diff( event_data->tecla );

    snprintf ( mensaje, FSM_MS_SIZE, FSM_MSG_T, event_data->tecla, dif );

    /* envio */
    post( &obj_1, mensaje );
}
/*==================[fin del archivo]========================================*/
