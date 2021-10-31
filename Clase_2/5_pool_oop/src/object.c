/*=============================================================================
 * Copyright (c) 2021, Franco Bucafusco <franco_bucafusco@yahoo.com.ar>
 * 					   Martin N. Menendez <mmenendez@fi.uba.ar>
 * All rights reserved.
 * License: read LICENCE file
 * Date: 2021/10/03
 * Version: v1.2
 *===========================================================================*/

/*==================[inclusiones]============================================*/
#include "object.h"
#include "main.h"

/*==================[definiciones y macros]==================================*/

/*==================[definiciones de datos internos]=========================*/

/*==================[definiciones de datos externos]=========================*/



/*==================[declaraciones de funciones internas]====================*/

/*==================[declaraciones de funciones externas]====================*/

/*==================[funciones]====================*/

void imprimir( char* mensaje )
{
    uartWriteString( USED_UART, mensaje );
}

void objeto_crear( tObjeto* objeto )
{
    objeto->cola  = xQueueCreate( N_QUEUE,sizeof( tMensaje ) );

    configASSERT( objeto->cola != NULL );
}

void post( tObjeto* objeto,tMensaje mensaje )
{
    xQueueSend( objeto->cola, &mensaje, portMAX_DELAY );
}

void get( tObjeto* objeto,tMensaje* mensaje )
{
    xQueueReceive( objeto->cola, mensaje,portMAX_DELAY );
}