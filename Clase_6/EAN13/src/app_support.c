/*=============================================================================
 * Copyright (c) 2021, Franco Bucafusco <franco_bucafusco@yahoo.com.ar>
 * 					   Martin N. Menendez <mmenendez@fi.uba.ar>
 * All rights reserved.
 * License: Free
 * Date: 2021/05/03
 * Version: v1.3
 *===========================================================================*/

#include "app_support.h"
#include <string.h>

extern QueueHandle_t response_queue;

void Check_packet( activeObject_t* caller_ao, void* mensaje_a_procesar )
{
    int i=2;//indice arranca en uno porque el primer byte es el tamaño del string

    char* mensaje = (char*) mensaje_a_procesar;
    // string = (M2342342342440)
    // string = "ERRRO_("
    // option = string[2] = M

    int largo = mensaje[0];

    if (mensaje[1] != TAG_INICIAL)
    {
    	strcpy( mensaje_a_procesar, ERROR_TAG_INICIAL );
    	mensaje[0]=sizeof(ERROR_TAG_INICIAL); //Indico cuantos bytes tiene el string
		return;
    }

    if (mensaje[largo] != TAG_FINAL)
	{
		strcpy( mensaje_a_procesar, ERROR_TAG_FINAL );
		mensaje[0]=sizeof(ERROR_TAG_FINAL); //Indico cuantos bytes tiene el string
		return;
	}

    while ( largo > i+1 )
    {

        if ( ( mensaje[i]>='0' )&&( mensaje[i]<='9' ) ) //si el caracter esta entre a y z los paso a mayuscula
        {
            //string[i]=string[i]+( 'A'-'a' ); // mayus
        	//string[i]=string[i]-( 'A'-'a' ); // minus
        }
        else
        {
            strcpy( mensaje_a_procesar, ERROR_NUMERICO );
            mensaje[0]=sizeof(ERROR_NUMERICO); //Indico cuantos bytes tiene el string
            return;
        }
        i++;
    }

    xQueueSend ( response_queue , &mensaje , 0); // "return" de activeObjectResponse_t
}

void Sietizador( activeObject_t* caller_ao, void* mensaje_a_procesar )
{
    int i=2;//indice arranca en uno porque el primer byte es el tamaño del string

    char* mensaje = (char*) mensaje_a_procesar;
    // string = (M2342342342440)
    // string = "ERRRO_("
    // option = string[2] = M

    int largo = mensaje[0];

    if (mensaje[1] != TAG_INICIAL)
    {
    	strcpy( mensaje, ERROR_TAG_INICIAL );
    	mensaje[0]=sizeof(ERROR_TAG_INICIAL); //Indico cuantos bytes tiene el string
		return;
    }

    if (mensaje[largo] != TAG_FINAL)
	{
		strcpy( mensaje_a_procesar, ERROR_TAG_FINAL );
		mensaje[0]=sizeof(ERROR_TAG_FINAL); //Indico cuantos bytes tiene el string
		return;
	}

    while ( largo > i+1 )
    {
    	// a-z o A-Z o ' '
        if ( ( mensaje[i]>='0' )&&( mensaje[i]<='9' ) ) //si el caracter esta entre a y z los paso a mayuscula
        {
            //string[i]=string[i]+(mensaje_a_procesar[i] == ' ')*( 'A'-'a' ); // mayus
        	//string[i]=string[i]-(mensaje_a_procesar[i] == ' ')*( 'A'-'a' ); // minus
        	mensaje[i] = 7 + '0'; // '7'
        }
        else
        {
            strcpy( mensaje_a_procesar, ERROR_NUMERICO );
            mensaje[0]=sizeof(ERROR_NUMERICO); //Indico cuantos bytes tiene el string
            return;
        }
        i++;
    }

    xQueueSend ( response_queue, &mensaje , 0); // "return" de activeObjectResponse_t
}

char getCommand( char* string )
{
	return ( string[2] );
}




