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

//  ES INVALIDO QUE LA IMNPLEMNTACIO DEL OBJETO CONOZCA ACERCA DE LAS INSTANCIAS.

/*==================[declaraciones de funciones internas]====================*/

/*==================[declaraciones de funciones externas]====================*/

/*==================[funciones]====================*/

void imprimir( char* mensaje )
{
    uartWriteString( USED_UART, mensaje );
}

tObjeto* objeto_crear()
{
    tObjeto* rv ;

    rv = pvPortMalloc( sizeof( tObjeto ) );

    configASSERT( rv != NULL );

    rv->cola  = xQueueCreate( N_QUEUE, sizeof( tMensaje ) );

    configASSERT( rv->cola != NULL );

    return rv;
}

void post( tObjeto* objeto, tMensaje mensaje )
{
    xQueueSend( objeto->cola, &mensaje, portMAX_DELAY );
}

void get( tObjeto* objeto,tMensaje* mensaje )
{
    xQueueReceive( objeto->cola, mensaje,portMAX_DELAY );
}

void objeto_delete( tObjeto* obj )
{
    /* primero se destryen los objetos "hijos"*/
    vQueueDelete( obj->cola );

    /* al final el obj */
    vPortFree( obj );
}