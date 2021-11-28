# RTOS2
# [**Recorda completar la encuestra clase a clase**](https://forms.gle/RvphazRZdpaU6VWG8)

Material didáctico para la asignatura Sistemas Operativos en Tiempo Real 2 de la Carrera de Especialización en Sistemas Embebidos.

## Compatibilidad de los ejercicios

Los ejecicios son compatibles con el [framework firmware v3](https://github.com/epernia/firmware_v3).

## Trabajo practico

El trabajo practico se publicará en la semana 2.

Para formar los grupos, se deberá utilizar [este](https://forms.gle/4VeFoNuXjGsgPJFz6) formulario.

[Lista de requerimientos.](https://docs.google.com/spreadsheets/d/1-VyaQY0eDLpg12Eqkxe7_bfCb77LKIbDfVTNDGFBpu0/edit?usp=sharing)

## Scripts

## Script de loopback
El test de loopback genera N mensajes aleatorios y los envia, validando luego lo recibido.
La validación se realiza comparando y validando que lo enviado sea igual a lo recibido. 

Sirve para probar el trabajo práctico antes de la implementación de C3. 

* Configurar config.py con los parametros que se desee.
* Ejecutar python3 test_random_loopback.py

## Script random
El test random genera una serie de mensajes aleatorios y los envia, validando luego lo recibido. 
En este caso, se cierra el loop en la C3, por lo tanto se puede probar en las etapas en donde 3 ya se encuentra implemnetada y, postriormente, con el agregado de objectos activos. 

* Configurar config.py con los parametros que se desee.
* Ejecutar python3 test_random.py

## Script test unitarios
El test valida las reglas del protocolo, sin exigir la ejecucion en terminos temporales. 
* Ejecutar python3 test_unitario.py

# Colaboradores

* Diego Essaya y Santiago Abbate (script random version inicial)
* Lucas Orsi (script test unitarios version inicial)

## Lecturas recomendadas

- [Modularidad, abstracción y múltiples instancias en C para Embedded Software](https://embedded-exploited.blogspot.com/2014/04/generalizacion-encapsulamiento-abstraccion-modularidad-oop.html)
- [Object-Oriented Programming in C](https://www.state-machine.com/doc/AN_OOP_in_C.pdf)
- [Developing Reusable Firmware – A Practical Approach to APIs, HALs and Drivers](https://www.beningo.com/store/developing-reusable-firmware-a-practical-approach-to-apis-hals-and-drivers/) Jacob Beningo ISBN-13 (pbk): 978-1-4842-3296-5
- [Gestión de Memoria Dinámica en Sistemas de Tiempo Real](http://www.gii.upv.es/tlsf/files/papers/mmasmano_phdthesis.pdf)
- [Practical Design Patterns: Opaque Pointers and Objects in C](https://interrupt.memfault.com/blog/opaque-pointers)
