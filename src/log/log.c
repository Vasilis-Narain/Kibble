#include "log.h"

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdarg.h>

#include "FreeRTOS.h" // IWYU pragma: keep
#include "queue.h"
#include "task.h"

#ifndef LOG_MAX_MSG_LEN
#define LOG_MAX_MSG_LEN 64
#endif /* LOG_MAX_MSG_LEN */

typedef struct {
    char msg[LOG_MAX_MSG_LEN];
} LogMsg;

static QueueHandle_t log_q;

static void log_task(void *pvParams) {
    (void)pvParams;
    LogMsg received;

    for (;;) {
        //receive from  q
        (void)xQueueReceive(log_q, &received, portMAX_DELAY);

        printf("%s", received.msg);
    }
}

//NOTE(vasilis): any task that calls this function must consider
//that it allocates `LOG_MAX_MSG_LEN` (default 64) bytes on its stack.
void log_printf(const char *fmt, ...) {
    if (log_q == NULL) {
        return;
    }

    LogMsg out;
    int32_t n = 0;
    va_list ap;

    va_start(ap, fmt);
    n = vsnprintf(out.msg, sizeof(out.msg), fmt, ap);
    va_end(ap);
    if (n < 0) {
        return;
    }

    (void)xQueueSend(log_q, &out, 0);
}

//NOTE(vasilis): recommended to use this sparringly
void log_printf_FromISR(const char *fmt, ...) {
    if (log_q == NULL) {
        return;
    }

    LogMsg out;
    int32_t n = 0;
    va_list ap;

    va_start(ap, fmt);
    n = vsnprintf(out.msg, sizeof(out.msg), fmt, ap);
    va_end(ap);
    if (n < 0) {
        return;
    }

    BaseType_t woken = pdFALSE;
    (void)xQueueSendFromISR(log_q, &out, &woken);
    portYIELD_FROM_ISR(woken);
}

bool log_init(void) {
    log_q = xQueueCreate(5, sizeof(LogMsg));

    if (log_q == NULL) {
        return false;
    }

    if (xTaskCreate(log_task, "log_task", 512, NULL, 1, NULL) == pdFAIL) {
        vQueueDelete(log_q);
        log_q = NULL;
        return false;
    }

    return true;
}
