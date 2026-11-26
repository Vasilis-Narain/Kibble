/*
 * SPDX-FileCopyrightText: 2026 Vasilis Narain
 *
 * SPDX-License-Identifier: MIT
 */
#include "display.h"

#include "fmc.h"

#define DISPLAY_BUS_WIDTH FMC_NORSRAM_MEM_BUS_WIDTH_8
#define DISPLAY_FMC_BASE 0x60000000u // NE1 bank base
#define DC_ADDR_LINE 16

#if (DISPLAY_BUS_WIDTH == FMC_NORSRAM_MEM_BUS_WIDTH_8)
#define CMD_ADDR ((volatile uint8_t *)DISPLAY_FMC_BASE)
#define DATA_ADDR ((volatile uint8_t *)(DISPLAY_FMC_BASE + (1u << DC_ADDR_LINE)))
#elif (DISPLAY_BUS_WIDTH == FMC_NORSRAM_MEM_BUS_WIDTH_16)
#define CMD_ADDR ((volatile uint16_t *)DISPLAY_FMC_BASE)
#define DATA_ADDR ((volatile uint16_t *)(DISPLAY_FMC_BASE + (1u << (DC_ADDR_LINE + 1u))))
#else
#error "FMC bus width must be 8 or 16"
#endif
