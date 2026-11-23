/*
 * SPDX-FileCopyrightText: 2026 Vasilis Narain
 *
 * SPDX-License-Identifier: MIT
 */

// TODO(vasilis): tmp template file to be filled with platform specific implementations

#include "ff.h"     /* Basic definitions of FatFs */
#include "diskio.h" /* Declarations FatFs MAI */

/* Example: Declarations of the platform and disk functions in the project */
#include "sdmmc.h"
#include "stm32h7xx_hal_gpio.h"
#include "stm32h7xx_hal_sd.h"

/* Example: Mapping of physical drive number for each drive */
#define DEV_FLASH 0 /* Map FTL to physical drive 0 */
#define DEV_MMC 1   /* Map MMC/SD card to physical drive 1 */
#define DEV_USB 2   /* Map USB MSD to physical drive 2 */

SD_HandleTypeDef *sd_handle = &hsd1;
static DSTATUS stat = STA_NOINIT;
/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status(
    BYTE pdrv /* Physical drive nmuber to identify the drive */
) {
    (void)pdrv;

    return stat;
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

    //NOTE(vasilis): hangs on fail, that's why we check the sd_det pin first
    //to make sure the sd is inserted. SD getting removed while running
    //will later be handled by EXTI isr
    MX_SDMMC1_SD_Init();

    return 0;
}

// TODO(vasilis): all the rest of the functions
#if 0
/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read(
    BYTE pdrv,    /* Physical drive nmuber to identify the drive */
    BYTE *buff,   /* Data buffer to store read data */
    LBA_t sector, /* Start sector in LBA */
    UINT count    /* Number of sectors to read */
) {
    DRESULT res;
    int result;

    switch (pdrv) {
    case DEV_RAM:
        // translate the arguments here

        result = RAM_disk_read(buff, sector, count);

        // translate the reslut code here

        return res;

    case DEV_MMC:
        // translate the arguments here

        result = MMC_disk_read(buff, sector, count);

        // translate the reslut code here

        return res;

    case DEV_USB:
        // translate the arguments here

        result = USB_disk_read(buff, sector, count);

        // translate the reslut code here

        return res;
    }

    return RES_PARERR;
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
    DRESULT res;
    int result;

    switch (pdrv) {
    case DEV_RAM:
        // translate the arguments here

        result = RAM_disk_write(buff, sector, count);

        // translate the reslut code here

        return res;

    case DEV_MMC:
        // translate the arguments here

        result = MMC_disk_write(buff, sector, count);

        // translate the reslut code here

        return res;

    case DEV_USB:
        // translate the arguments here

        result = USB_disk_write(buff, sector, count);

        // translate the reslut code here

        return res;
    }

    return RES_PARERR;
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
    DRESULT res;
    int result;

    switch (pdrv) {
    case DEV_RAM:

        // Process of the command for the RAM drive

        return res;

    case DEV_MMC:

        // Process of the command for the MMC/SD card

        return res;

    case DEV_USB:

        // Process of the command the USB drive

        return res;
    }

    return RES_PARERR;
}
#endif
