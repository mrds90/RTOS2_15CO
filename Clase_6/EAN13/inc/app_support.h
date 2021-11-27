/*=============================================================================
 * Copyright (c) 2020, Martin N. Menendez <menendezmartin81@gmail.com>
 * All rights reserved.
 * License: Free
 * Date: 2020/07/26
 * Version: v1.1
 *===========================================================================*/

#ifndef __MAYUS_MINUS_H__
#define __MAYUS_MINUS_H__

#include "checksum.h"
#include "AO.h"

#define TAG_INICIAL '('
#define TAG_FINAL ')'
#define ERROR_TAG_INICIAL " Tag inicial incorrecto"
#define ERROR_TAG_FINAL " Tag final incorrecto"
#define ERROR_NUMERICO " Todos los datos deben ser NUMEROS"

#define PRIMER_DIGITO '0'
#define ULTIMO_DIGITO '9'

#define POS_LARGO 0
#define POS_TAG_INICIAL 1

void Check_packet( activeObject_t* caller_ao, void* mensaje_a_procesar );
void Sietizador( activeObject_t* caller_ao, void* mensaje_a_procesar );
char getCommand( char* string );

#endif
