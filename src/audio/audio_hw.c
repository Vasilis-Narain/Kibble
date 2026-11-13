#include "audio.h"
#include <string.h>

#include "FreeRTOS.h" // IWYU pragma: keep
#include "task.h"
#include "main.h"
#include "sai.h"

static SAI_HandleTypeDef *const audio_handle = &hsai_BlockA1;
static TaskHandle_t audio_task_handle;

#define AUDIO_HALF_FRAMES 256u
#define AUDIO_HALF_SAMPLES (AUDIO_HALF_FRAMES * 2u)
__attribute__((section(".RAM_NOCACHE"), aligned(32))) static int16_t audio_buf[AUDIO_HALF_SAMPLES * 2u];

static void audio_task(void *arg) {
    (void)arg;

    // init buffer to 0 (RAM_NOCACHE is NOLOAD)
    memset(audio_buf, 0, sizeof(audio_buf));

    // init  dma
    if (HAL_SAI_Transmit_DMA(audio_handle, (uint8_t *)audio_buf, AUDIO_HALF_SAMPLES * 2) != HAL_OK) {
        Error_Handler();
    }

    // unmute speaker
    HAL_GPIO_WritePin(AMP_SD_GPIO_Port, AMP_SD_Pin, GPIO_PIN_SET);

    for (;;) {
        uint32_t half;
        xTaskNotifyWait(0, 0, &half, portMAX_DELAY);

        // TODO(vasilis): mixing logic
        // tmp fill with square wave
        static uint32_t phase = 0;
        static int16_t level = 8000;

        int16_t *out = &audio_buf[half * AUDIO_HALF_SAMPLES];
        for (uint32_t i = 0; i < AUDIO_HALF_SAMPLES; ++i) {
            out[2 * i] = level;
            out[2 * i + 1] = level;
            if (++phase == 55) {
                phase = 0;
                level = -level;
            }
        }
    }

    vTaskDelete(NULL);
}

static void audio_buffer_done(uint32_t free_half) {
    BaseType_t woken = pdFALSE;
    xTaskNotifyFromISR(audio_task_handle, free_half, eSetValueWithOverwrite, &woken);
    portYIELD_FROM_ISR(woken);
}

// irq callbacks
void HAL_SAI_TxHalfCpltCallback(SAI_HandleTypeDef *handle) {
    (void)handle;
    audio_buffer_done(0);
}

void HAL_SAI_TxCpltCallback(SAI_HandleTypeDef *handle) {
    (void)handle;
    audio_buffer_done(1);
}

void audio_init(void) {
    if (xTaskCreate(audio_task, "audio_task", 512, NULL, 6, &audio_task_handle) != pdPASS) {
        Error_Handler();
    }
}
