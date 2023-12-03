#pragma once

// Class created to define needed functions that are used in main.c
// Without this header it is not possible to start the software
// It also contains some HEAP regions that are defined in heap_5.c

// Created by: Rod, Moran

#ifndef RTOS_HELPER
#define RTOS_HELPER

#define mainREGION_1_SIZE                     8201
#define mainREGION_2_SIZE                     23905
#define mainREGION_3_SIZE                     16807

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <stdbool.h>

/* Visual studio intrinsics used so the __debugbreak() function is available
 * should an assert get hit. */
#include <intrin.h>

 /* FreeRTOS kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include <semphr.h>
#include "trcRecorder.h"

StackType_t uxTimerTaskStack[configTIMER_TASK_STACK_DEPTH];

void vAssertCalled(unsigned long ulLine,
    const char* const pcFileName)
{
    static BaseType_t xPrinted = pdFALSE;
    volatile uint32_t ulSetToNonZeroInDebuggerToContinue = 0;
    (void)ulLine;
    (void)pcFileName;

    taskENTER_CRITICAL();
    {
        printf("ASSERT! Line %ld, file %s, GetLastError() %ld\r\n", ulLine, pcFileName, GetLastError());
        (void)xTraceDisable();

        //__debugbreak();
        while (ulSetToNonZeroInDebuggerToContinue == 0)
        {
            __asm {
                NOP
            };
            __asm {
                NOP
            };
        }
        (void)xTraceEnable(TRC_START);
    }
    taskEXIT_CRITICAL();
}

static uint32_t ulEntryTime = 0;

void vTraceTimerReset(void)
{
    ulEntryTime = xTaskGetTickCount();
}

uint32_t uiTraceTimerGetFrequency(void)
{
    return configTICK_RATE_HZ;
}

uint32_t uiTraceTimerGetValue(void)
{
    return(xTaskGetTickCount() - ulEntryTime);
}

void vApplicationTickHook(void)
{
    return 0;
}

void vApplicationDaemonTaskStartupHook(void)
{
    return 0;
}

void vApplicationGetIdleTaskMemory(StaticTask_t** ppxIdleTaskTCBBuffer,
    StackType_t** ppxIdleTaskStackBuffer,
    uint32_t* pulIdleTaskStackSize)
{
    static StaticTask_t xIdleTaskTCB;
    static StackType_t uxIdleTaskStack[configMINIMAL_STACK_SIZE];
    *ppxIdleTaskTCBBuffer = &xIdleTaskTCB;
    *ppxIdleTaskStackBuffer = uxIdleTaskStack;
    *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}

void vApplicationGetTimerTaskMemory(StaticTask_t** ppxTimerTaskTCBBuffer,
    StackType_t** ppxTimerTaskStackBuffer,
    uint32_t* pulTimerTaskStackSize)
{
    static StaticTask_t xTimerTaskTCB;
    *ppxTimerTaskTCBBuffer = &xTimerTaskTCB;
    *ppxTimerTaskStackBuffer = uxTimerTaskStack;
    *pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}

void vApplicationIdleHook(void)
{
    return 0;
}

void vApplicationMallocFailedHook(void)
{
    vAssertCalled(__LINE__, __FILE__);
}

static void prvInitialiseHeap(void)
{
    /* The Windows demo could create one large heap region, in which case it would
     * be appropriate to use heap_4.  However, purely for demonstration purposes,
     * heap_5 is used instead, so start by defining some heap regions.  No
     * initialisation is required when any other heap implementation is used.  See
     * http://www.freertos.org/a00111.html for more information.
     *
     * The xHeapRegions structure requires the regions to be defined in start address
     * order, so this just creates one big array, then populates the structure with
     * offsets into the array - with gaps in between and messy alignment just for test
     * purposes. */
    static uint8_t ucHeap[configTOTAL_HEAP_SIZE];
    volatile uint32_t ulAdditionalOffset = 19; /* Just to prevent 'condition is always true' warnings in configASSERT(). */
    const HeapRegion_t xHeapRegions[] =
    {
        /* Start address with dummy offsets						Size */
        { ucHeap + 1,                                          mainREGION_1_SIZE },
        { ucHeap + 15 + mainREGION_1_SIZE,                     mainREGION_2_SIZE },
        { ucHeap + 19 + mainREGION_1_SIZE + mainREGION_2_SIZE, mainREGION_3_SIZE },
        { NULL,                                                0                 }
    };

    /* Sanity check that the sizes and offsets defined actually fit into the
     * array. */
    configASSERT((ulAdditionalOffset + mainREGION_1_SIZE + mainREGION_2_SIZE + mainREGION_3_SIZE) < configTOTAL_HEAP_SIZE);

    /* Prevent compiler warnings when configASSERT() is not defined. */
    (void)ulAdditionalOffset;

    vPortDefineHeapRegions(xHeapRegions);
}
#endif // !RTOS_HELPER