/*=============================================================================
 * Copyright (c) 2021, Franco Bucafusco <franco_bucafusco@yahoo.com.ar>
 * 					   Martin N. Menendez <mmenendez@fi.uba.ar>
 * All rights reserved.
 * License: read LICENCE file
 * Date: 2021/10/03
 * Version: v1.2
 *===========================================================================*/

#ifndef OBJECT_H_
#define OBJECT_H_

/*==================[inclusiones]============================================*/
#include "FreeRTOS.h"
#include "task.h"
#include "sapi.h"
#include "semphr.h"
#include <string.h>
#include "FreeRTOSConfig.h"

/*==================[definiciones y macros]==================================*/
typedef char* tMensaje; // String

typedef struct
{
    QueueHandle_t cola;
} tObjeto;

void imprimir( char* mensaje );
void objeto_crear( tObjeto* objeto );
void post( tObjeto* objeto,tMensaje mensaje );
void get( tObjeto* objeto,tMensaje* mensaje );


#endif /* OBJECT_H_ */
