#include "app.h"

#include <stdio.h>

#include "FreeRTOS.h" // IWYU pragma: keep
#include "task.h"
#include "main.h"
#include "input.h"

void vApplicationMallocFailedHook(void) {
    configASSERT(0);
}

void vApplicationStackOverflowHook(TaskHandle_t t, char *n) {
    (void)t;
    (void)n;
    configASSERT(0);
}

void frame_loop(void *pvParameters) {
    TickType_t last_wake_time = xTaskGetTickCount();
    static uint32_t frame = 0;
    for (;;) {
        input_poll();

        if (input_is_key_pressed(INPUT_A)) {
            printf("Input A is pressed\r\n");
        }
        if (input_is_key_pressed(INPUT_JOY_SEL)) {
            printf("Input JOY_SEL is pressed\r\n");
        }

        if (++frame % 30 == 0) {
            HAL_GPIO_TogglePin(PWR_LED_GPIO_Port, PWR_LED_Pin);
            int32_t x = (int32_t)(input_get_joy_axis_movement(AXIS_X) * 100);
            int32_t y = (int32_t)(input_get_joy_axis_movement(AXIS_Y) * 100);
            printf("%ld %ld\r\n", (long)x, (long)y);
        }

        vTaskDelayUntil(&last_wake_time, pdMS_TO_TICKS(17));
    }

    vTaskDelete(NULL);
}

void app_main(void) {
    input_joystick_init();

    xTaskCreate(frame_loop, "frame_loop", 256, NULL, 1, NULL);
    vTaskStartScheduler();

    for (;;) {}
}
