/*=============================================================================
 * Copyright (c) 2021, Franco Bucafusco <franco_bucafusco@yahoo.com.ar>
 * 					   Martin N. Menendez <mmenendez@fi.uba.ar>
 * All rights reserved.
 * License: read LICENCE file
 * Date: 2021/10/03
 * Version: v1.2
 *===========================================================================*/

#ifndef USER_TASKS_H_
#define USER_TASKS_H_
#define MSG_LED                 "LED ON"
#define FSM_MSG_T               "TEC%u T%u"          //TODO refactor
#define FSM_MSG_EXAMPLE         "TECx Tyyyy"         //TODO refactor

#define MSG_LED_SIZE            sizeof(MSG_LED)
#define FSM_MS_SIZE             sizeof(FSM_MSG_EXAMPLE)

#define MAX_MESSAGE_SIZE        (MSG_LED_SIZE>FSM_MS_SIZE?MSG_LED_SIZE:FSM_MS_SIZE)

#define btn_format(x) 			x-TEC1+1
#define time_format(x) 			x/portTICK_RATE_MS

/*==================[declaraciones de funciones internas]====================*/
void gpio_init( void );
/*==================[declaraciones de tareas]====================*/
void tarea_A( void* taskParmPtr );
void tarea_C( void* taskParmPtr );

#endif /* USER_TASKS_H_ */
