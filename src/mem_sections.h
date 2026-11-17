#ifndef __MEM_SECTIONS_H
#define __MEM_SECTIONS_H

#ifdef __arm__
#define AXI_BSS __attribute__((section(".AXI_BSS")))
#define DMA_NOCACHE __attribute__((section(".RAM_NOCACHE"), aligned(32)))
#else // __arm__
#define AXI_BSS
#define DMA_NOCACHE
#endif // __arm__

#endif // __MEM_SECIONS_H
