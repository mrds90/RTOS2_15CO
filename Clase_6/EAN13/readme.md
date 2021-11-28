# Cambios

- deje comentarios por ahi TODO:REVIEW TODO:INFO
- unir uartirq.c y driver.c en protocol.c /* aqui puede llamarse c2 si se quiere. */
- borre un array llamado uartData
- cambie driver_t por protocol_t
- cambie selectedUart por selected_protocol
- renombre driverInit por protocol_init
- rename Uart_driver por protocol
- rename packetTX por protocol_send
- rename waitPacket por protocol_wait_frame
- rename usertask.c por app.c
- rename mayusc.c por app_support.c
- puse configUSE_MUTEXES en 0 porque no usa ...
       configUSE_RECURSIVE_MUTEXES
       configUSE_MALLOC_FAILED_HOOK
       configUSE_COUNTING_SEMAPHORES
       configUSE_TRACE_FACILITY
       configSUPPORT_STATIC_ALLOCATION
       INCLUDE_vTaskPrioritySet
       INCLUDE_uxTaskPriorityGet
- quite putBuffer porque no se usaba
        getBuffer


- cree app_create para tener a esta capa hecha un objeto
- rename checkTask por app_task


# cambios de version con ao

- meti los oas dentro de la estructura de app_t
- hice un refactor heavy metal