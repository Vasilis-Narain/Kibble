#include "log.h"

#include "FreeRTOS.h" // IWYU pragma: keep
#include "queue.h"

QueueHandle_t log_q;

void log_init(void) {
    log_q = xQueueCreate(5, sizeof(LogMsg));
}
