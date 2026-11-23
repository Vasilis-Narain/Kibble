/*
 * SPDX-FileCopyrightText: 2026 Vasilis Narain
 *
 * SPDX-License-Identifier: MIT
 */

#include "ff.h"     /* Basic definitions of FatFs */
#include "diskio.h" /* Declarations FatFs MAI */

/* Example: Declarations of the platform and disk functions in the project */
#include "sdmmc.h"
#include "stm32h7xx_hal.h"
#include "stm32h7xx_hal_gpio.h"
#include "stm32h7xx_hal_sd.h"

#define SD_TIMEOUT 30 * 1000
#define SD_DEFAULT_BLOCK_SIZE 512

SD_HandleTypeDef *sd_handle = &hsd1;

static volatile DSTATUS stat = STA_NOINIT;
static volatile UINT write_status = 0;
static volatile UINT read_status = 0;

/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

static DSTATUS sd_check_card_state(BYTE pdrv) {
    (void)pdrv;
    stat = STA_NOINIT;

    if (HAL_SD_GetCardState(sd_handle) == HAL_SD_STATE_READY) {
        stat &= ~STA_NOINIT;
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
    GPIO_PinState sd_det_state = HAL_GPIO_ReadPin(SD_DET_GPIO_Port, SD_DET_Pin);
    if (sd_det_state == GPIO_PIN_RESET) {
        stat = STA_NODISK | STA_NOINIT;
        return stat;
    }

    //NOTE(vasilis): mimics the void MX_SDMMC1_SD_Init(void) function
    sd_handle->Instance = SDMMC1;
    sd_handle->Init.ClockEdge = SDMMC_CLOCK_EDGE_RISING;
    sd_handle->Init.ClockPowerSave = SDMMC_CLOCK_POWER_SAVE_DISABLE;
    sd_handle->Init.BusWide = SDMMC_BUS_WIDE_4B;
    sd_handle->Init.HardwareFlowControl = SDMMC_HARDWARE_FLOW_CONTROL_ENABLE;
    sd_handle->Init.ClockDiv = 14;
    if (HAL_SD_Init(sd_handle) != HAL_OK) {
        stat = STA_NOINIT;
    } else {
        stat = 0;
    }

    return stat;
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
        // NOTE(vasilis):read_status updated by isr
        while ((read_status == 0) && ((HAL_GetTick() - timeout) < SD_TIMEOUT)) {}

        if (read_status == 0) { //means loop failed for timeout
            res = RES_ERROR;
            HAL_SD_Abort(sd_handle);
        } else {
            read_status = 0;
            timeout = HAL_GetTick();

            while ((HAL_GetTick() - timeout) < SD_TIMEOUT) {
                if (HAL_SD_GetCardState(sd_handle) == HAL_SD_CARD_READY) {
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
        //NOTE(vasilis): write_status gets updated by isr
        while ((write_status == 0) && ((HAL_GetTick() - timeout) < SD_TIMEOUT)) {}

        if (write_status == 0) {
            res = RES_ERROR;
            HAL_SD_Abort(sd_handle);
        } else {
            write_status = 0;
            timeout = HAL_GetTick();

            while ((HAL_GetTick() - timeout) < SD_TIMEOUT) {
                if (HAL_SD_GetCardState(sd_handle) == HAL_SD_CARD_READY) {
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
