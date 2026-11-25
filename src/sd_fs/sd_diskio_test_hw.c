/*
 * SPDX-FileCopyrightText: 2026 Vasilis Narain
 *
 * SPDX-License-Identifier: MIT
 */
#include <string.h>
#include "mem_sections.h"
#include "ff.h"
#include "log.h"
#include "sdmmc.h"

DMA_NOCACHE FATFS SDFatFs; // filesystem object
DMA_NOCACHE FIL test_file; // file object

#define LONG_ALIGNED 1028
#define LONG_UNALIGNED 1029
#define SHORT_ALIGNED 512
#define SHORT_UNALIGNED 511

static bool test_sd_card(const char *name, uint8_t *ptr, uint32_t len);

static AXI_BSS uint8_t axi_long_aligned[LONG_ALIGNED];
static AXI_BSS uint8_t axi_long_unaligned[LONG_UNALIGNED];
static AXI_BSS uint8_t axi_short_aligned[SHORT_ALIGNED];
static AXI_BSS uint8_t axi_short_unaligned[SHORT_UNALIGNED];

static DMA_NOCACHE uint8_t nocache_long_aligned[LONG_ALIGNED];
static DMA_NOCACHE uint8_t nocache_long_unaligned[LONG_UNALIGNED];
static DMA_NOCACHE uint8_t nocache_short_aligned[SHORT_ALIGNED];
static DMA_NOCACHE uint8_t nocache_short_unaligned[SHORT_UNALIGNED];

static uint8_t dtcm_long_aligned[LONG_ALIGNED];
static uint8_t dtcm_long_unaligned[LONG_UNALIGNED];
static uint8_t dtcm_short_aligned[SHORT_ALIGNED];
static uint8_t dtcm_short_unaligned[SHORT_UNALIGNED];

static uint8_t write_text_long_aligned[LONG_ALIGNED];
static uint8_t write_text_long_unaligned[LONG_UNALIGNED];

static uint8_t write_text_short_aligned[SHORT_ALIGNED];
static uint8_t write_text_short_unaligned[SHORT_UNALIGNED];

static uint8_t *ptr_table_long_aligned[] = {
    axi_long_aligned,
    dtcm_long_aligned,
    nocache_long_aligned,
};
static uint8_t *ptr_table_long_unaligned[] = {
    axi_long_unaligned,
    dtcm_long_unaligned,
    nocache_long_unaligned,
};

static uint8_t *ptr_table_short_aligned[] = {
    axi_short_aligned,
    dtcm_short_aligned,
    nocache_short_aligned,
};
static uint8_t *ptr_table_short_unaligned[] = {
    axi_short_unaligned,
    dtcm_short_unaligned,
    nocache_short_unaligned,
};

static const char *mems[] = {"AXI", "DTCM", "NOCACHE"};

bool sd_diskio_hw_test(void) {
    bool res = true;
    for (int i = 0; i < 3; ++i) {
        if (!test_sd_card(mems[i], ptr_table_long_aligned[i], LONG_ALIGNED) ||
            !test_sd_card(mems[i], ptr_table_long_unaligned[i], LONG_UNALIGNED) ||
            !test_sd_card(mems[i], ptr_table_short_aligned[i], SHORT_ALIGNED) ||
            !test_sd_card(mems[i], ptr_table_short_unaligned[i], SHORT_UNALIGNED)) {
            res = false;
        }
    }
    if (res != false) {
        log_printf("SD tests succeded.\r\nFinal readback visual check (32 chars): ");
        dtcm_short_unaligned[32] = '\0';
        log_printf("%s\r\n", (char *)dtcm_short_unaligned);
    } else {
        log_printf("SD tests failed!\r\n");
    }
    return res;
}

static bool test_sd_card(const char *name, uint8_t *ptr, uint32_t len) {
    uint8_t *write_text;
    switch (len) {
    case LONG_ALIGNED:
        write_text = write_text_long_aligned;
        break;
    case LONG_UNALIGNED:
        write_text = write_text_long_unaligned;
        break;
    case SHORT_ALIGNED:
        write_text = write_text_short_aligned;
        break;
    case SHORT_UNALIGNED:
        write_text = write_text_short_unaligned;
        break;
    default:
        LOG("invalid 'len' input argument\r\n");
        return false;
    }
    uint8_t *read_buffer = ptr;
    FRESULT res;
    uint32_t bytes_read;
    uint32_t bytes_written;

    memset(read_buffer, 0, len);
    memset(write_text, 'A', len);
    // initial mount
    res = f_mount(&SDFatFs, "", 1);
    if (res != FR_OK) {
        log_printf("%s:%u:failed initial mount: res = %d, hsd1.e = %d\r\n", name, len, res, hsd1.ErrorCode);
        return false;
    }
    // open + write
    res = f_open(&test_file, "test.txt", FA_CREATE_ALWAYS | FA_WRITE);
    if (res != FR_OK) {
        log_printf("%s:%u:failed initial open: res = %d, hsd1.e = %d\r\n", name, len, res, hsd1.ErrorCode);
        return false;
    }
    res = f_write(&test_file, write_text, len, (void *)&bytes_written);
    if ((bytes_written > 0) && (res == FR_OK)) {
        f_close(&test_file);
    } else {
        f_close(&test_file);
        return false;
    }

    // unmount then mount to ensure we read from sd
    f_unmount("");
    res = f_mount(&SDFatFs, "", 1);
    if (res != FR_OK) {
        log_printf("%s:%u:failed second mount: res = %d, hsd1.e = %d\r\n", name, len, res, hsd1.ErrorCode);
        return false;
    }

    // then open + read
    res = f_open(&test_file, "test.txt", FA_READ);
    if (res != FR_OK) {
        log_printf("%s:%u:failed second open: res = %d, hsd1.e = %d\r\n", name, len, res, hsd1.ErrorCode);
        return false;
    }
    res = f_read(&test_file, read_buffer, len, (void *)&bytes_read);
    if ((bytes_read > 0) && (res == FR_OK)) {
        f_close(&test_file);
        if (bytes_read == bytes_written) {
            if (memcmp(read_buffer, write_text, len) != 0) {
                log_printf("%s:%u:failed `memcmp()`\r\n", name, len);
                return false;
            }
            return true;
        }
    }
    return false;
}
