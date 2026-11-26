/*
 * SPDX-FileCopyrightText: 2026 Vasilis Narain
 *
 * SPDX-License-Identifier: MIT
 */

// On-device test for the FatFs disk I/O glue (sd_diskio_hw.c).
// Only built when app.c includes it (FS_TEST).
//
// Each case fills a buffer with a pattern, writes it to a file, clears the buffer, reads
// the file back into the same buffer and checks it. Buffers live in each memory region at
// offsets chosen to force every disk_read/disk_write path:
//   offset 0: 32-byte aligned -> direct DMA (cached AXI: clean/invalidate)
//   offset 4: word aligned    -> cached AXI: write direct, read via scratch
//   offset 1: unaligned       -> cached AXI: scratch both ways
// DTCM is not reachable by IDMA, so it always goes via scratch.
//
// FatFs only hands the caller's buffer to disk_read/disk_write for whole sectors, so the
// lengths are whole sectors (single and multi block). The extra 5 bytes in the long case
// go through FatFs's own buffer.
//
// Canary bytes around each buffer catch DMA or cache maintenance writing outside it.

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "ff.h"
#include "log.h"
#include "mem_sections.h"
#include "sdmmc.h"

#define SD_TEST_SECTOR 512u
#define SD_TEST_MAX_LEN (2u * SD_TEST_SECTOR + 5u)
#define SD_TEST_SLACK 32u // room before the buffer for the offsets, and after it
#define SD_TEST_REGION_SIZE (SD_TEST_SLACK + SD_TEST_MAX_LEN + SD_TEST_SLACK)
#define SD_TEST_CANARY 0x5Au
#define SD_TEST_FILE "sdtest.bin"

typedef struct {
    const char *name;
    uint8_t *base; // 32-byte aligned
} sd_test_region_t;

static DMA_NOCACHE FATFS sd_test_fs;
static DMA_NOCACHE FIL sd_test_file;

static AXI_BSS __attribute__((aligned(32))) uint8_t sd_test_axi[SD_TEST_REGION_SIZE];
static DMA_NOCACHE uint8_t sd_test_nocache[SD_TEST_REGION_SIZE];
static __attribute__((aligned(32))) uint8_t sd_test_dtcm[SD_TEST_REGION_SIZE];

static const sd_test_region_t sd_test_regions[] = {
    {"AXI", sd_test_axi},
    {"NOCACHE", sd_test_nocache},
    {"DTCM", sd_test_dtcm},
};
static const uint32_t sd_test_offsets[] = {0u, 4u, 1u};
static const uint32_t sd_test_lengths[] = {SD_TEST_SECTOR, SD_TEST_MAX_LEN};

#define SD_TEST_COUNT(a) (sizeof(a) / sizeof((a)[0]))

// Position dependent and different per case, so data from the wrong sector, a shifted
// offset or a previous case doesn't match.
static uint8_t sd_test_pattern(uint32_t i, uint32_t seed) {
    return (uint8_t)(i + (i >> 8) * 37u + seed * 101u);
}

static void sd_test_log_fail(const char *name, uint8_t *buf, uint32_t len, const char *step, FRESULT res) {
    log_printf("%s a%lu n%lu: %s fail res=%d err=0x%08lx\r\n", name, (unsigned long)((uint32_t)buf & 31u),
               (unsigned long)len, step, (int)res, (unsigned long)hsd1.ErrorCode);
}

// Returns true if every byte outside [buf, buf + len) still holds the canary.
static bool sd_test_canaries_ok(const sd_test_region_t *region, uint8_t *buf, uint32_t len) {
    for (uint8_t *p = region->base; p < region->base + SD_TEST_REGION_SIZE; ++p) {
        if ((p >= buf) && (p < buf + len)) {
            continue;
        }
        if (*p != SD_TEST_CANARY) {
            log_printf("%s a%lu n%lu: canary hit at buf%+ld\r\n", region->name, (unsigned long)((uint32_t)buf & 31u),
                       (unsigned long)len, (long)(p - buf));
            return false;
        }
    }
    return true;
}

static bool sd_test_case(const sd_test_region_t *region, uint32_t offset, uint32_t len, uint32_t seed) {
    uint8_t *buf = region->base + SD_TEST_SLACK + offset;
    FRESULT res;
    FRESULT close_res;
    UINT bytes_written = 0;
    UINT bytes_read = 0;

    memset(region->base, SD_TEST_CANARY, SD_TEST_REGION_SIZE);
    for (uint32_t i = 0; i < len; ++i) {
        buf[i] = sd_test_pattern(i, seed);
    }

    // write from the buffer under test
    res = f_open(&sd_test_file, SD_TEST_FILE, FA_CREATE_ALWAYS | FA_WRITE);
    if (res != FR_OK) {
        sd_test_log_fail(region->name, buf, len, "open w", res);
        return false;
    }
    res = f_write(&sd_test_file, buf, len, &bytes_written);
    close_res = f_close(&sd_test_file); // flushes the partial last sector
    if (res != FR_OK) {
        sd_test_log_fail(region->name, buf, len, "write", res);
        return false;
    }
    if (close_res != FR_OK) {
        sd_test_log_fail(region->name, buf, len, "close w", close_res);
        return false;
    }
    if (bytes_written != len) {
        sd_test_log_fail(region->name, buf, len, "short write", FR_OK);
        return false;
    }

    // remount so nothing can come from FatFs's cached state
    f_unmount("");
    res = f_mount(&sd_test_fs, "", 1);
    if (res != FR_OK) {
        sd_test_log_fail(region->name, buf, len, "remount", res);
        return false;
    }

    // read back into the same buffer
    memset(buf, 0, len);
    res = f_open(&sd_test_file, SD_TEST_FILE, FA_READ);
    if (res != FR_OK) {
        sd_test_log_fail(region->name, buf, len, "open r", res);
        return false;
    }
    res = f_read(&sd_test_file, buf, len, &bytes_read);
    close_res = f_close(&sd_test_file);
    if (res != FR_OK) {
        sd_test_log_fail(region->name, buf, len, "read", res);
        return false;
    }
    if (close_res != FR_OK) {
        sd_test_log_fail(region->name, buf, len, "close r", close_res);
        return false;
    }
    if (bytes_read != len) {
        sd_test_log_fail(region->name, buf, len, "short read", FR_OK);
        return false;
    }

    for (uint32_t i = 0; i < len; ++i) {
        uint8_t expected = sd_test_pattern(i, seed);
        if (buf[i] != expected) {
            log_printf("%s a%lu n%lu: data at %lu got 0x%02x want 0x%02x\r\n", region->name,
                       (unsigned long)((uint32_t)buf & 31u), (unsigned long)len, (unsigned long)i, buf[i], expected);
            return false;
        }
    }

    return sd_test_canaries_ok(region, buf, len);
}

bool sd_diskio_hw_test(void) {
    uint32_t total = 0;
    uint32_t failed = 0;
    uint32_t seed = 0;

    FRESULT res = f_mount(&sd_test_fs, "", 1);
    if (res != FR_OK) {
        log_printf("SD test: mount fail res=%d err=0x%08lx\r\n", (int)res, (unsigned long)hsd1.ErrorCode);
        return false;
    }

    for (uint32_t r = 0; r < SD_TEST_COUNT(sd_test_regions); ++r) {
        for (uint32_t o = 0; o < SD_TEST_COUNT(sd_test_offsets); ++o) {
            for (uint32_t l = 0; l < SD_TEST_COUNT(sd_test_lengths); ++l) {
                ++total;
                ++seed;
                if (!sd_test_case(&sd_test_regions[r], sd_test_offsets[o], sd_test_lengths[l], seed)) {
                    ++failed;
                }
            }
        }
    }

    f_unlink(SD_TEST_FILE);
    f_unmount("");

    if (failed == 0) {
        log_printf("SD test: all %lu cases passed\r\n", (unsigned long)total);
    } else {
        log_printf("SD test: %lu of %lu cases FAILED\r\n", (unsigned long)failed, (unsigned long)total);
    }
    return failed == 0;
}
