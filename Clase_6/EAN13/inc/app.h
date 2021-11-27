/*=============================================================================
 * Copyright (c) 2020, Martin N. Menendez <menendezmartin81@gmail.com>
 * All rights reserved.
 * License: Free
 * Date: 2020/07/26
 * Version: v1.1
 *===========================================================================*/

/*=====[Avoid multiple inclusion - begin]====================================*/

#ifndef __USER_APP_H__
#define __USER_APP_H__

/*=====[Inclusions of public function dependencies]==========================*/
#include <stdbool.h>
#include "protocol.h"
#include "FreeRTOS.h"
#include "task.h"
#include "sapi.h"
#include "AO.h"
/*=====[C++ - begin]=========================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*=====[Definition macros of public constants]===============================*/

#define TASK_DELAY pdMS_TO_TICKS(100)
#define TICK_LED 	LEDG
#define CHECK_LED 	LED3

/*=====[Public function-like macros]=========================================*/

/*=====[Definitions of public data types]====================================*/

typedef struct
{
    void* app_task_hnd; //hnd a la tarea de freertos
    void* protocol_hnd; //hnd a "c2"
} app_t;


/*=====[Prototypes (declarations) of public functions]=======================*/

void tickTask( void* taskParmPtr );  // Task declaration

bool_t app_create( app_t* app_instance, void* protocol );

/*=====[Prototypes (declarations) of public interrupt functions]=============*/

/*=====[C++ - end]===========================================================*/

#ifdef __cplusplus
}
#endif

/*=====[Avoid multiple inclusion - end]======================================*/

#endif
