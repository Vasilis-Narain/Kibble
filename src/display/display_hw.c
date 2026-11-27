/*
 * SPDX-FileCopyrightText: 2026 Vasilis Narain
 *
 * SPDX-License-Identifier: MIT
 */
#include "display.h"
#include "display_commands.h"

#include <stdint.h>

#include "main.h"
#include "stm32h7xx_hal.h"

//#include "fmc.h"
//#include "mdma.h"

//#include "mem_sections.h"
#include "stm32h7xx_hal_gpio.h"
#include "stm32h7xx_hal_tim.h"
#include "tim.h"

#define DISPLAY_BUS_WIDTH FMC_NORSRAM_MEM_BUS_WIDTH_8
#define DISPLAY_FMC_BASE 0x60000000u // NE1 bank base
#define DC_ADDR_LINE 16
#define FB_SIZE_16BYTE (SCREEN_WIDTH * SCREEN_HEIGHT)

#if (DISPLAY_BUS_WIDTH == FMC_NORSRAM_MEM_BUS_WIDTH_8)
#define CMD_ADDR ((volatile uint8_t *)DISPLAY_FMC_BASE)
#define DATA_ADDR ((volatile uint8_t *)(DISPLAY_FMC_BASE + (1u << DC_ADDR_LINE)))
#elif (DISPLAY_BUS_WIDTH == FMC_NORSRAM_MEM_BUS_WIDTH_16)
#define CMD_ADDR ((volatile uint16_t *)DISPLAY_FMC_BASE)
#define DATA_ADDR ((volatile uint16_t *)(DISPLAY_FMC_BASE + (1u << (DC_ADDR_LINE + 1u))))
#else
#error "FMC bus width must be 8 or 16"
#endif

// private functions:
static inline void display_write_cmd(ST7789VI_Cmd cmd);
static inline void display_write_data(uint16_t data);
static inline void display_hw_reset(void);

//NOTE(vasilis): suppress unused warnings
#if 0
// private variables
static MDMA_HandleTypeDef *mdma_handle = &hmdma_mdma_channel0_sw_0; // mdma handle alias
static volatile uint32_t transfer_error_detected = 0;               // Tx Error detected flag (isr sets)
static volatile uint32_t transfer_complete_detected = 0;            // Tx Cplt detected flag (isr sets)

// Frame buffer (tx buffer)
// NOTE(vasilis): pixel x,y is fb[y * SCREEN_WIDTH + x]
static DMA_NOCACHE uint16_t fb[FB_SIZE_16BYTE];

// tmp naive buffer filling
static void fill_buffer(uint16_t *ptr, uint32_t len, uint16_t data) {
    for (uint16_t i = 0; i < len; ++i) {
        ptr[i] = i + data;
    }
}
#endif

#define DISPLAY_MIN_RESET_DELAY_MS 120u
#define DCMD_COLMOD_RGB_LSB 4
#define DCMD_COLMOD_BITS_LSB 0
#define DCMD_COLMOD_BITS_16 (0x5 << DCMD_COLMOD_BITS_LSB)
#define DCMD_COLMOD_RGB_65K (0x5 << DCMD_COLMOD_RGB_LSB)

#define DCMD_MADCTL_MY_BITS (1u << 7)  // page address order
#define DCMD_MADCTL_MX_BITS (1u << 6)  // column address order
#define DCMD_MADCTL_MV_BITS (1u << 5)  // page/column order
#define DCMD_MADCTL_ML_BITS (1u << 4)  // line address order
#define DCMD_MADCTL_RGB_BITS (1u << 3) // rgb/bgr order
#define DCMD_MADCTL_MH_BITS (1u << 2)  // display data latch order

// runs  display init sequence
// NOTE(vasilis): this function blocks -- heavily (use at init, as the name suggests)
void display_init(void) {
    // Hardware reset
    display_hw_reset();

    // Sleep out
    display_write_cmd(DCMD_SLPOUT);
    HAL_Delay(DISPLAY_MIN_RESET_DELAY_MS);

    // Display orientation -> transpose + flip (for landscape)
    display_write_cmd(DCMD_MADCTL);
    display_write_data(DCMD_MADCTL_MV_BITS | DCMD_MADCTL_MX_BITS);

    // Pixel format -> RGB565
    display_write_cmd(DCMD_COLMOD);
    display_write_data(DCMD_COLMOD_RGB_65K | DCMD_COLMOD_BITS_16);

    // Inversion on
    display_write_cmd(DCMD_INVON);

    // Tuning settings from manufacturer (https://support.newhavendisplay.com/hc/en-us/articles/4413934921879-2-4-TFT-ST7789-SPI-Controller):
    // Porch setting
    display_write_cmd(DCMD_PORCTRL);
    display_write_data(0x0c);
    display_write_data(0x0c);
    display_write_data(0x00);
    display_write_data(0x33);
    display_write_data(0x33);

    // Gate control
    display_write_cmd(DCMD_GCTRL);
    display_write_data(0x35);

    // VCOM setting
    display_write_cmd(DCMD_VCOMS);
    display_write_data(0x2b);

    // LCM control
    display_write_cmd(DCMD_LCMCTRL);
    display_write_data(0x2c);

    // VDV and VRH Command Enable
    display_write_cmd(DCMD_VDVVRHEN);
    display_write_data(0x01);
    display_write_data(0xff);

    // VRH set
    display_write_cmd(DCMD_VRHS);
    display_write_data(0x11);

    // VDV set
    display_write_cmd(DCMD_VDVS);
    display_write_data(0x20);

    // Frame Rate control in normal mode
    display_write_cmd(DCMD_FRCTRL2);
    display_write_data(0x0f); // 60Hz

    // Power Control 1
    display_write_cmd(DCMD_PWCTRL1);
    display_write_data(0xa4);
    display_write_data(0xa1);

    // Positive voltage gamma control
    display_write_cmd(DCMD_PVGAMCTRL);
    display_write_data(0xd0);
    display_write_data(0x00);
    display_write_data(0x05);
    display_write_data(0x0e);
    display_write_data(0x15);
    display_write_data(0x0d);
    display_write_data(0x37);
    display_write_data(0x43);
    display_write_data(0x47);
    display_write_data(0x09);
    display_write_data(0x15);
    display_write_data(0x12);
    display_write_data(0x16);
    display_write_data(0x19);

    // Negative voltage gamma control
    display_write_cmd(DCMD_NVGAMCTRL);
    display_write_data(0xd0);
    display_write_data(0x00);
    display_write_data(0x05);
    display_write_data(0x0d);
    display_write_data(0x0c);
    display_write_data(0x06);
    display_write_data(0x2d);
    display_write_data(0x44);
    display_write_data(0x40);
    display_write_data(0x0e);
    display_write_data(0x1c);
    display_write_data(0x18);
    display_write_data(0x16);
    display_write_data(0x19);
    // end of manufacturer tuning settings

    // Endianness fix (NOTE(vasilis): for Prototype only as final will use 16-bit direct)
    display_write_cmd(DCMD_RAMCTRL);
    display_write_data(0x00);
    display_write_data(0x08); // only sets the relevant bit

    // X address start/end -> sets 0..319
    display_write_cmd(DCMD_CASET);
    display_write_data(0x00);
    display_write_data(0x00);
    display_write_data(0x00);
    display_write_data(SCREEN_WIDTH - 1u);

    // Y address start/end -> sets 0..239
    display_write_cmd(DCMD_RASET);
    display_write_data(0x00);
    display_write_data(0x00);
    display_write_data(0x00);
    display_write_data(SCREEN_HEIGHT - 1u);

    // Display on
    display_write_cmd(DCMD_DISPON);

    // TE pin on
    //display_write_cmd(DCMD_TEON);
    //display_write_data(0x00); // V-Blanking only (one pulse per frame on end)

    HAL_Delay(10);

    HAL_TIM_PWM_Start(&htim14, TIM_CHANNEL_1);
    __HAL_TIM_SET_COMPARE(&htim14, TIM_CHANNEL_1, 30); // value from 0 to 60
}

static inline void display_hw_reset(void) {
    HAL_GPIO_WritePin(LCD_RST_GPIO_Port, LCD_RST_Pin, GPIO_PIN_RESET);
    HAL_Delay(1); // make sure rst is not rejected
    HAL_GPIO_WritePin(LCD_RST_GPIO_Port, LCD_RST_Pin, GPIO_PIN_SET);
    HAL_Delay(DISPLAY_MIN_RESET_DELAY_MS);
}

// 'manual' writing cmd/data needed for init sequence
// After that MDMA interface takes over
static inline void display_write_cmd(ST7789VI_Cmd cmd) {
    *CMD_ADDR = cmd;
}

// 'manual' writing cmd/data needed for init sequence
// After that MDMA interface takes over
static inline void display_write_data(uint16_t data) {
    *DATA_ADDR = data;
}
