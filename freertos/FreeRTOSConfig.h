#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

#include <stdint.h>
extern uint32_t SystemCoreClock; /* kept up to date by the HAL (system_stm32h7xx.c) */

/* ---- Core ---------------------------------------------------------------- */
#define configCPU_CLOCK_HZ (SystemCoreClock)
#define configTICK_RATE_HZ 1000 /* 1 ms tick */
#define configUSE_PREEMPTION 1
#define configUSE_TIME_SLICING 1
#define configMAX_PRIORITIES 8
#define configMINIMAL_STACK_SIZE 256 /* in WORDS (4 bytes) → 1 KB, idle task */
#define configMAX_TASK_NAME_LEN 16
#define configTICK_TYPE_WIDTH_IN_BITS TICK_TYPE_WIDTH_32_BITS
#define configIDLE_SHOULD_YIELD 1
#define configUSE_TASK_NOTIFICATIONS 1
#define configUSE_MUTEXES 1
#define configUSE_RECURSIVE_MUTEXES 0
#define configUSE_COUNTING_SEMAPHORES 1
#define configQUEUE_REGISTRY_SIZE 0
#define configUSE_PORT_OPTIMISED_TASK_SELECTION 1 /* CLZ instruction, M7 has it */

/* ---- Memory -------------------------------------------------------------- */
#define configSUPPORT_DYNAMIC_ALLOCATION 1
#define configSUPPORT_STATIC_ALLOCATION 0 /* 1 needs extra callbacks for idle/timer task memory */
#define configTOTAL_HEAP_SIZE (32 * 1024) /* heap_4 array → .bss → DTCM. NO DMA buffers from here! */

/* ---- Hooks / debugging ---------------------------------------------------- */
#define configUSE_IDLE_HOOK 0
#define configUSE_TICK_HOOK 0
#define configUSE_MALLOC_FAILED_HOOK 1   /* you implement vApplicationMallocFailedHook() */
#define configCHECK_FOR_STACK_OVERFLOW 2 /* you implement vApplicationStackOverflowHook() */
#define configASSERT(x)                \
    do {                               \
        if (!(x)) {                    \
            __asm volatile("cpsid i"); \
            for (;;) {}                \
        }                              \
    } while (0)

/* ---- Software timers ------------------------------------------------------ */
#define configUSE_TIMERS 1
#define configTIMER_TASK_PRIORITY (configMAX_PRIORITIES - 1)
#define configTIMER_QUEUE_LENGTH 10
#define configTIMER_TASK_STACK_DEPTH 256

/* ---- Interrupt priorities (STM32H7: 4 priority bits, __NVIC_PRIO_BITS = 4) ---- */
#define configPRIO_BITS 4
#define configLIBRARY_LOWEST_INTERRUPT_PRIORITY 15
#define configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY 5 /* ISRs at 5..15 may call ...FromISR() */
#define configKERNEL_INTERRUPT_PRIORITY (configLIBRARY_LOWEST_INTERRUPT_PRIORITY << (8 - configPRIO_BITS))
#define configMAX_SYSCALL_INTERRUPT_PRIORITY (configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY << (8 - configPRIO_BITS))

/* ---- Map the port's handlers onto the CMSIS vector names ------------------ */
/* CubeMX must NOT generate SVC_Handler, PendSV_Handler, SysTick_Handler. */
#define vPortSVCHandler SVC_Handler
#define xPortPendSVHandler PendSV_Handler
#define xPortSysTickHandler SysTick_Handler

/* ---- Optional API functions ------------------------------------------------ */
#define INCLUDE_vTaskDelay 1
#define INCLUDE_xTaskDelayUntil 1
#define INCLUDE_vTaskDelete 1
#define INCLUDE_vTaskSuspend 1
#define INCLUDE_xTaskGetSchedulerState 1
#define INCLUDE_uxTaskGetStackHighWaterMark 1

#endif /* FREERTOS_CONFIG_H */
