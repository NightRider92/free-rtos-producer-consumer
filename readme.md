**Producer - Consumer simple implementation**
* Be advised - project needs to be linked with FreeRTOS headers and sources *

**Detailed info:**
This is a producer-consumer FreeRTOS (real-time operating system) simple project that performs 
read/write operations into random memory locations.

This example uses semaphore to protect 
the access to variables / buffer and to prevent racing conditions and also notification variable 
(bool) that is used for signalization.

Example is made in C language (low-level) in FreeRTOS simulator
which is available here: https://www.freertos.org/FreeRTOS-simulator-for-Windows.html

Running software:
![image](https://github.com/NightRider92/free-rtos-producer-consumer/assets/10942663/a123e822-bbf6-4232-aea9-7c70e882bca7)






