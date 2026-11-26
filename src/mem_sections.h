/*
 * SPDX-FileCopyrightText: 2026 Vasilis Narain
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef KIBBLE_MEM_SECTIONS_H
#define KIBBLE_MEM_SECTIONS_H

#include <stdint.h>
#include <stdbool.h>

extern uint8_t _snocache[];
extern uint8_t _enocache[];
extern uint8_t _saxi_cached[];
extern uint8_t _eaxi_cached[];

#ifdef __arm__
#define AXI_BSS __attribute__((section(".AXI_BSS")))                      // NOLOAD
#define DMA_NOCACHE __attribute__((section(".RAM_NOCACHE"), aligned(32))) // NOLOAD

//NOTE(vasilis): use `len = 0` to just check if `ptr` is in `DMA_NOCACHE`
static inline bool mem_buff_is_nocache(const void *ptr, uint32_t len) {
    uint32_t ptr_n = (uint32_t)ptr;
    if (ptr_n < (uint32_t)_snocache || ptr_n >= (uint32_t)_enocache) {
        return false;
    }
    if (len > (uint32_t)_enocache - ptr_n) {
        return false;
    }
    return true;
}

//NOTE(vasilis): use `len = 0` to just check if `ptr` is in `AXI_BSS`
static inline bool mem_buff_is_axi_cached(const void *ptr, uint32_t len) {
    uint32_t ptr_n = (uint32_t)ptr;
    if (ptr_n < (uint32_t)_saxi_cached || ptr_n >= (uint32_t)_eaxi_cached) {
        return false;
    }
    if (len > (uint32_t)_eaxi_cached - ptr_n) {
        return false;
    }
    return true;
}

#else // __arm__
#define AXI_BSS
#define DMA_NOCACHE

static inline bool mem_buff_is_nocache(const void *ptr, uint32_t len) {
    (void)ptr;
    (void)len;
    return true;
}

static inline bool mem_buff_is_axi_cached(const void *ptr, uint32_t len) {
    (void)ptr;
    (void)len;
    return true;
}
#endif // __arm__

// Stops compiler from reordering loads
#define COMPILER_FENCE __atomic_signal_fence(__ATOMIC_SEQ_CST)

#endif // KIBBLE_MEM_SECTIONS_H
