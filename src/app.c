/*
 * SPDX-FileCopyrightText: 2026 Vasilis Narain
 *
 * SPDX-License-Identifier: MIT
 */

#include "app.h"

#include "FreeRTOS.h" // IWYU pragma: keep
#include "task.h"
#include "main.h"
#include "input.h"
#include "audio.h"
#include "log.h"

void vApplicationMallocFailedHook(void) {
    configASSERT(0);
}

void vApplicationStackOverflowHook(TaskHandle_t t, char *n) {
    (void)t;
    (void)n;
    configASSERT(0);
}

static void frame_loop(void *pvParameters) {
    (void)pvParameters;

    TickType_t last_wake_time = xTaskGetTickCount();
    static uint32_t frame = 0;
    for (;;) {
        input_poll();

        // we don't want printf in actual tasks.
        // create gatekeeper with a q
        if (input_is_key_pressed(INPUT_A)) {
            log_printf("Input A is pressed\r\n");
        }
        if (input_is_key_pressed(INPUT_JOY_SEL)) {
            log_printf("Input JOY_SEL is pressed\r\n");
        }

        if (++frame % 30 == 0) {
            HAL_GPIO_TogglePin(PWR_LED_GPIO_Port, PWR_LED_Pin);
            int32_t x = (int32_t)(input_get_joy_axis_movement(AXIS_X) * 100);
            int32_t y = (int32_t)(input_get_joy_axis_movement(AXIS_Y) * 100);
            log_printf("%ld %ld\r\n", (long)x, (long)y);
        }

        vTaskDelayUntil(&last_wake_time, pdMS_TO_TICKS(17));
    }

    vTaskDelete(NULL);
}

void app_main(void) {
    input_joystick_init();
    audio_init();

    if (log_init() == false) {
        Error_Handler();
    }

    if (xTaskCreate(frame_loop, "frame_loop", 512, NULL, 2, NULL) != pdPASS) {
        // NOTE(vasilis): fail here means insufficient heap memory for task
        Error_Handler();
    }
    vTaskStartScheduler();

    for (;;) {}
}
