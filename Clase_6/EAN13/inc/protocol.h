/*=============================================================================
 * Copyright (c) 2020, Martin N. Menendez <menendezmartin81@gmail.com>
 * All rights reserved.
 * License: Free
 * Date: 2020/07/26
 * Version: v1.1
 *===========================================================================*/

/*=====[Avoid multiple inclusion - begin]====================================*/

#ifndef __DRIVER_H__
#define __DRIVER_H__

/*=====[Inclusions of public function dependencies]==========================*/

#include "checksum.h"
#include "qmpool.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "string.h"
#include "sapi.h"
#include "timers.h"

/*=====[C++ - begin]=========================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*=====[Definition macros of public constants]===============================*/

#define PROTOCOL_TIMEOUT        pdMS_TO_TICKS(3)
#define PACKET_SIZE             127 //dejo un byte para tamaño del paquete y que quede redondo en 128. Maximo 254
#define BLOCK_SIZE              (PACKET_SIZE+1)
#define POOL_TOTAL_BLOCKS       4//catidad de bloques en el pool de memoria
#define POOL_SIZE               POOL_TOTAL_BLOCKS*BLOCK_SIZE //Tamaño total del pool


/*=====[Public function-like macros]=========================================*/

/*=====[Definitions of public data types]====================================*/

typedef struct
{
    uartMap_t uartValue;				// handler del la uart que se utilizara con sapi
    uint32_t baudRate; 					//baudrate		 /* configuracio de la uart NOTA: puede no estar */

    char *Pool_pointer; //puntero al segmento de memoria que albergara el pool para transmision
    QMPool Pool; //memory pool (contienen la informacion que necesita la biblioteca qmpool.h

    /* Forma est�tica
    char Pool_pointer[POOL_SIZE * sizeof( char )]; //Y NO SE TIENE Q LLAMAR A MALLOC LUEGO.
    QMPool Pool;
    */

    TimerHandle_t onTxTimeOut; 		//Timer para el timeout de la transmision
    TimerHandle_t onRxTimeOut; 		//Timer para el timeout de la recepcion

    /* info del paquete que se esta transmitiendo */
    char *txBlock;      		//Bloque de memoria de la transmision en curso
    uint8_t txCounter;  	//cuenta los bytes transmitidos para el bloque apuntado por txBlock
    uint8_t txLen;     			 //longitud del paquete del bloque apuntado por txBlock

    /* info del paquete que se esta recibiendo */
    char *rxBlock;      		//Bloque de memoria de la recepcion
    uint8_t rxLen;      		//cuenta los bytes recibido en el bloque apuntado por rxBlock

    QueueHandle_t onTxQueue; 		//cola de transmision (por esta cola llegan los bloques de memoria a transmitir)
    QueueHandle_t onRxQueue; 		//cola de recepcion (por esta cola llegan los bloques de memoria recibidos)
} protocol_t;

/*=====[Prototypes (declarations) of public functions]=======================*/

bool_t	protocol_init	( protocol_t* );


void 	protocol_send 	( protocol_t* selected_protocol,char *txBlock );
char* 	protocol_wait_frame	( protocol_t* );

/*=====[Prototypes (declarations) of public interrupt functions]=============*/

/*=====[C++ - end]===========================================================*/

#ifdef __cplusplus
}
#endif

/*=====[Avoid multiple inclusion - end]======================================*/

#endif /
