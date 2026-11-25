/*
 * SPDX-FileCopyrightText: 2026 Vasilis Narain
 *
 * SPDX-License-Identifier: MIT
 */

/* This file contains glue function implementations used by FatFS */
#include "sd.h"

#include "ff.h"     /* Basic definitions of FatFs */
#include "diskio.h" /* Declarations FatFs MAI */

#include "main.h"
#include "sdmmc.h"
#include "stm32h7xx_hal.h"

#include "log.h"
#include "mem_sections.h"

#define SD_TIMEOUT 1000 // 1 second timeout
#define SD_DEFAULT_BLOCK_SIZE 512
#define SD_DEBOUNCE_MS 10
#define SD_TRANSFER_BUS_WIDTH 1

#if defined(SD_TRANSFER_BUS_WIDTH) && (SD_TRANSFER_BUS_WIDTH == 1)
#define SD_TRANSFER_MODE_INTERNAL SDMMC_BUS_WIDE_1B
#elif defined(SD_TRANSFER_BUS_WIDTH) && (SD_TRANSFER_BUS_WIDTH == 4)
#define SD_TRANSFER_MODE_INTERNAL SDMMC_BUS_WIDE_4B
#else
#error "SD Bus width must be 1 or 4 bits wide!"
#endif

//private variables:
static SD_HandleTypeDef *const sd_handle = &hsd1;
// status variables:
static volatile DSTATUS stat = STA_NOINIT;
static volatile UINT write_status = 0;
static volatile UINT read_status = 0;

// debounce variables:
static GPIO_PinState sd_candidate = GPIO_PIN_RESET;
static GPIO_PinState sd_stable = GPIO_PIN_RESET;
static uint32_t sd_changed_at = 0;

/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/
static GPIO_PinState sd_detect_stable(void) {
    GPIO_PinState raw_sd = HAL_GPIO_ReadPin(SD_DET_GPIO_Port, SD_DET_Pin);
    uint32_t now = HAL_GetTick();

    if (raw_sd != sd_candidate) {
        sd_candidate = raw_sd;
        sd_changed_at = now;
    }

    if ((uint32_t)(now - sd_changed_at) >= SD_DEBOUNCE_MS) {
        sd_stable = sd_candidate;
    }
    return sd_stable;
}

static DSTATUS sd_check_card_state(BYTE pdrv) {
    (void)pdrv;

    if (sd_detect_stable() == GPIO_PIN_RESET) {
        stat = STA_NOINIT | STA_NODISK;
    }

    return stat;
}

DSTATUS disk_status(
    BYTE pdrv /* Physical drive nmuber to identify the drive */
) {
    return sd_check_card_state(pdrv);
}

/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize(
    BYTE pdrv /* Physical drive nmuber to identify the drive */
) {
    (void)pdrv;
    // Early return if sd_det is held low (meaning sd is not present)
    GPIO_PinState raw_sd = HAL_GPIO_ReadPin(SD_DET_GPIO_Port, SD_DET_Pin);
    if (raw_sd == GPIO_PIN_RESET) {
        stat = STA_NODISK | STA_NOINIT;
        sd_candidate = raw_sd;
        sd_stable = raw_sd;
        return stat;
    }

    //NOTE(vasilis): mimics the void MX_SDMMC1_SD_Init(void) function
    sd_handle->Instance = SDMMC1;
    sd_handle->Init.ClockEdge = SDMMC_CLOCK_EDGE_RISING;
    sd_handle->Init.ClockPowerSave = SDMMC_CLOCK_POWER_SAVE_DISABLE;
    sd_handle->Init.BusWide = SD_TRANSFER_MODE_INTERNAL;
    sd_handle->Init.HardwareFlowControl = SDMMC_HARDWARE_FLOW_CONTROL_ENABLE;

    //NOTE(vasilis): Clock input (pre div) is 140 MHZ
    sd_handle->Init.ClockDiv = 14;
    if (HAL_SD_Init(sd_handle) != HAL_OK) {
        stat = STA_NOINIT;
    } else {
        sd_candidate = raw_sd;
        sd_stable = raw_sd;
        stat = 0;
    }

    return stat;
}

//NOTE(vasilis): small helper that spins until the isr's update status_ptr, the sd throws error, or timeout
//Returns `HAL_SD_ERROR_` status codes.
static inline uint32_t sd_transfer_cplt_status_poll(volatile UINT *status_ptr, uint32_t start_time) {
    uint32_t err = HAL_SD_ERROR_NONE;
    while ((*status_ptr == 0) && ((err = HAL_SD_GetError(sd_handle)) == 0) && ((HAL_GetTick() - start_time) < SD_TIMEOUT)) {}
    if (err == HAL_SD_ERROR_NONE && *status_ptr == 0) {
        return HAL_SD_ERROR_TIMEOUT;
    }
    return err;
}

/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read(
    BYTE pdrv,    /* Physical drive nmuber to identify the drive */
    BYTE *buff,   /* Data buffer to store read data */
    LBA_t sector, /* Start sector in LBA */
    UINT count    /* Number of sectors to read */
) {
    (void)pdrv;
    DRESULT res = RES_ERROR;
    uint32_t timeout;
    read_status = 0;

    if (HAL_SD_ReadBlocks_DMA(sd_handle, (uint8_t *)buff, (uint32_t)sector, count) == HAL_OK) {
        // Wait for reading process to complete (or timeout)
        timeout = HAL_GetTick();
        uint32_t err = sd_transfer_cplt_status_poll(&read_status, timeout);

        if (err != HAL_SD_ERROR_NONE) { //means loop failed for timeout or sd err
            res = RES_ERROR;
            LOG("err: 0x%08lx\r\n", err);
            HAL_SD_Abort(sd_handle);
        } else {
            read_status = 0;
            timeout = HAL_GetTick();

            while ((HAL_GetTick() - timeout) < SD_TIMEOUT) {
                if (HAL_SD_GetCardState(sd_handle) == HAL_SD_CARD_TRANSFER) {
                    res = RES_OK;
                    //NOTE(vasilis): here is where we would invalidate the cache
                    break;
                }
            }
        }
    }

    return res;
}

/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if FF_FS_READONLY == 0

DRESULT disk_write(
    BYTE pdrv,        /* Physical drive nmuber to identify the drive */
    const BYTE *buff, /* Data to be written */
    LBA_t sector,     /* Start sector in LBA */
    UINT count        /* Number of sectors to write */
) {
    (void)pdrv;
    DRESULT res = RES_ERROR;
    write_status = 0;
    uint32_t timeout;

    if (HAL_SD_WriteBlocks_DMA(sd_handle, (uint8_t *)buff, (uint32_t)sector, count) == HAL_OK) {
        //wait for writing to complete (or timeout)
        timeout = HAL_GetTick();
        uint32_t err = sd_transfer_cplt_status_poll(&write_status, timeout);

        if (err != HAL_SD_ERROR_NONE) { //means loop failed for timeout or sd err
            res = RES_ERROR;
            LOG("err: 0x%08lx\r\n", err);
            HAL_SD_Abort(sd_handle);
        } else {
            write_status = 0;
            timeout = HAL_GetTick();

            while ((HAL_GetTick() - timeout) < SD_TIMEOUT) {
                if (HAL_SD_GetCardState(sd_handle) == HAL_SD_CARD_TRANSFER) {
                    res = RES_OK;
                    break;
                }
            }
        }
    }

    return res;
}

#endif

/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl(
    BYTE pdrv, /* Physical drive nmuber (0..) */
    BYTE cmd,  /* Control code */
    void *buff /* Buffer to send/receive control data */
) {
    (void)pdrv;
    DRESULT res = RES_ERROR;
    HAL_SD_CardInfoTypeDef card_info;

    if (stat & STA_NOINIT) {
        return RES_NOTRDY;
    }

    switch (cmd) {
    // no  pending write process
    case CTRL_SYNC:
        res = RES_OK;
        break;

    //get number of  sectors on disk (DWORD)
    case GET_SECTOR_COUNT:
        HAL_SD_GetCardInfo(sd_handle, &card_info);
        *(DWORD *)buff = card_info.LogBlockNbr;
        res = RES_OK;
        break;

    //get r/w sector size (WORD)
    case GET_SECTOR_SIZE:
        HAL_SD_GetCardInfo(sd_handle, &card_info);
        *(WORD *)buff = card_info.LogBlockSize;
        res = RES_OK;
        break;

    //get erase block size in unit of sector (DWORD)
    case GET_BLOCK_SIZE:
        HAL_SD_GetCardInfo(sd_handle, &card_info);
        *(DWORD *)buff = card_info.LogBlockSize / SD_DEFAULT_BLOCK_SIZE;
        res = RES_OK;
        break;

    // command not supported
    default:
        res = RES_PARERR;
    }

    return res;
}

//ISR's:

// Rx transfer completed
void HAL_SD_RxCpltCallback(SD_HandleTypeDef *hsd) {
    (void)hsd;
    read_status = 1;
}

// Tx transfer completed
void HAL_SD_TxCpltCallback(SD_HandleTypeDef *hsd) {
    (void)hsd;
    write_status = 1;
}

// SD Det callback
void sd_det_irq_callback(void) {} //NOTE(vasilis): tbd if this gets used
