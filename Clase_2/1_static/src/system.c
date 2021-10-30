#include <stdio.h>
#include <stdbool.h>
#include "FreeRTOS.h"
#include "task.h"

void sysAssert_Report_Hang( bool condicion, int line, const char* fcn )
{
    if( false == condicion )
    {
        taskENTER_CRITICAL();               //< critical section sin exit: la idea es colgar el sistema
        //gpioWrite( LED_ERROR, ON );
        //imprimir( MSG_ERROR_QUE );
        printf( "que lio! archivo %s linea %u\r\n", fcn, line );
        while( true );						// VER ESTE LINK: https://pbs.twimg.com/media/BafQje7CcAAN5en.jpg
    }
}

void sysAssert_Hang( bool condicion, int line, char* fcn )
{
    ( void )line; //not used, calms gcc
    ( void )fcn; //not used, calms gcc

    if( false == condicion )
    {
        taskENTER_CRITICAL();               //< critical section sin exit: la idea es colgar el sistema
        //gpioWrite( LED_ERROR, ON );
        //imprimir( MSG_ERROR_QUE );
        while( true );						// VER ESTE LINK: https://pbs.twimg.com/media/BafQje7CcAAN5en.jpg
    }
}

/**
   @brief Solo es llamado por el kernel si  configUSE_MALLOC_FAILED_HOOK == 1 en  FreeRTOSConfig.h

 */
void vApplicationMallocFailedHook( void )
{
    printf( "Application Malloc Failed Hook!\r\n" );
    configASSERT( false );
}