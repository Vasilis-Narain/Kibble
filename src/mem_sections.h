/*
 * SPDX-FileCopyrightText: 2026 Vasilis Narain
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef __MEM_SECTIONS_H
#define __MEM_SECTIONS_H

#ifdef __arm__
#define AXI_BSS __attribute__((section(".AXI_BSS")))
#define DMA_NOCACHE __attribute__((section(".RAM_NOCACHE"), aligned(32)))
#else // __arm__
#define AXI_BSS
#define DMA_NOCACHE
#endif // __arm__

#define COMPILER_FENCE __atomic_signal_fence(__ATOMIC_SEQ_CST) // compiler fence

#endif // __MEM_SECIONS_H
