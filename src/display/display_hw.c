/*
 * SPDX-FileCopyrightText: 2026 Vasilis Narain
 *
 * SPDX-License-Identifier: MIT
 */
#include "display.h"
#include "display_commands.h"

#include <stdint.h>

#include "stm32h7xx_hal.h"

#if 0
#include "fmc.h"
#include "mdma.h"

#include "mem_sections.h"
#endif

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
static void display_write_cmd(ST7789VI_Cmd cmd);
static void display_write_data(uint16_t data);

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

#define DISPLAY_MAX_RESET_DELAY_MS 120u

// runs  display init sequence
// NOTE(vasilis): this function blocks -- heavily (use at init, as the name suggests)
void display_init(void) {
    display_write_cmd(DCMD_SWRESET);
    HAL_Delay(DISPLAY_MAX_RESET_DELAY_MS);
    display_write_cmd(DCMD_SLPOUT);
    HAL_Delay(DISPLAY_MAX_RESET_DELAY_MS);
    display_write_cmd(DCMD_COLMOD);
    display_write_data(0x55u); // set to 16-bit pixel data
}

// NOTE(vasilis): 'manual' writing cmd/data needed for init sequence
// After that MDMA interface takes over
static void display_write_cmd(ST7789VI_Cmd cmd) {
    *CMD_ADDR = cmd;
}

// NOTE(vasilis): 'manual' writing cmd/data needed for init sequence
// After that MDMA interface takes over
static void display_write_data(uint16_t data) {
    *DATA_ADDR = data;
}
