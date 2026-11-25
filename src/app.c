/*
 * SPDX-FileCopyrightText: 2026 Vasilis Narain
 *
 * SPDX-License-Identifier: MIT
 */

#include "app.h"

// freertos
#include "FreeRTOS.h" // IWYU pragma: keep
#include "task.h"

// Fat fs
#include "main.h"
#include "input.h"
#include "audio.h"
#include "log.h"
#include "sd.h"

#define FS_TEST 1

#if (FS_TEST == 1)
#include "sd_fs/sd_diskio_test_hw.c"
#endif // FS_TEST

//NOTE(vasilis): tmp for on-device testing of Sfx system
#include "laser_shoot.h"
static Sfx laser;

// freeRTOS hooks
void vApplicationMallocFailedHook(void) {
    configASSERT(0);
}
void vApplicationStackOverflowHook(TaskHandle_t t, char *n) {
    (void)t;
    (void)n;
    configASSERT(0);
}

// GPIO IRQ dispatcher
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    switch (GPIO_Pin) {
    case SD_DET_Pin:
        sd_det_irq_callback();
        break;
    default: // not implemented pins
        break;
    }
}

//NOTE(vasilis): eventually the TE pin from the display will drive frame update
//targetting 60fps
static void frame_loop(void *pvParameters) {
    (void)pvParameters;

#if (FS_TEST == 1)
    sd_diskio_hw_test();
#endif // FS_TEST

    TickType_t last_wake_time = xTaskGetTickCount();
    static uint32_t frame = 0;
    for (;;) {
        input_poll();

        if (input_is_key_pressed(INPUT_A)) {
            log_printf("Input A is pressed\r\n");

            //NOTE(vasilis): tmp for on-device testing of Sfx system
            audio_play_sfx(laser);
        }
        if (input_is_key_pressed(INPUT_JOY_SEL)) {
            log_printf("Input JOY_SEL is pressed\r\n");

            //NOTE(vasilis): tmp for on-device testing of Sfx system
            audio_play_sfx(laser);
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

    //NOTE(vasilis): tmp for on-device testing of Sfx system
    laser = audio_sfx_load(laser_shoot, LASER_SHOOT_LEN, 1.0f, LASER_SHOOT_RATE, SFX_PCM16, 4);

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
