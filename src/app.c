/*
 * SPDX-FileCopyrightText: 2026 Vasilis Narain
 *
 * SPDX-License-Identifier: MIT
 */

#include "app.h"

// freertos
#include "FreeRTOS.h" // IWYU pragma: keep
#include "sdmmc.h"
#include "stm32h7xx_hal_gpio.h"
#include "task.h"

// Fat fs
#include <ff.h>

#include "mem_sections.h"
#include "main.h"
#include "input.h"
#include "audio.h"
#include "log.h"
#include "sd.h"

//NOTE(vasilis): tmp for on-device testing of Sfx system
#include "laser_shoot.h"
static Sfx laser;

//NOTE(vasilis): testing sd bring up
//////////////////////////////////////
DMA_NOCACHE FATFS SDFatFs; // filesystem object
DMA_NOCACHE FIL test_file; // file object
DMA_NOCACHE uint8_t read_buffer[64];

void test_sd_card(void) {
    FRESULT res;
    uint32_t bytes_read;
    uint32_t bytes_written;
    uint8_t write_text[] = "[STM32H743_Eval]: SD Init OK";

    // initial mount
    res = f_mount(&SDFatFs, "", 1);
    if (res != FR_OK) {
        log_printf("failed initial mount: res = %d, hsd1.e = %d\r\n", res, hsd1.ErrorCode);
    }
    // open + write
    res = f_open(&test_file, "test.txt", FA_CREATE_ALWAYS | FA_WRITE);
    if (res != FR_OK) {
        log_printf("failed initial open: res = %d, hsd1.e = %d\r\n", res, hsd1.ErrorCode);
    }
    res = f_write(&test_file, write_text, sizeof(write_text), (void *)&bytes_written);
    if ((bytes_written > 0) && (res == FR_OK)) {
        f_close(&test_file);
    }

    // unmount then mount to ensure we read from sd
    f_unmount("");
    res = f_mount(&SDFatFs, "", 1);
    if (res != FR_OK) {
        log_printf("failed second mount: res = %d, hsd1.e = %d\r\n", res, hsd1.ErrorCode);
    }

    // then open + read
    res = f_open(&test_file, "test.txt", FA_READ);
    if (res != FR_OK) {
        log_printf("failed second open: res = %d, hsd1.e = %d\r\n", res, hsd1.ErrorCode);
    }
    res = f_read(&test_file, read_buffer, sizeof(read_buffer), (void *)&bytes_read);
    if ((bytes_read > 0) && (res == FR_OK)) {
        f_close(&test_file);
        if (bytes_read == bytes_written) {
            return;
        }
    }
    return;
}

//////////////////////////////////////

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

    test_sd_card();
    log_printf("SD Readback: %s\r\n", (char *)read_buffer);

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
