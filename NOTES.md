# GENERAL NOTES

## Display

Data flow: dma2d reads sprites from sram and writes into the back buffer in AXI SRAM. When done
swap front and back buffer ptrs. MDMA (or DMA) then writes to FMC slave address which automatically initiates the 8080 transfer 
on write. This final transfer should be started no sooner than receiving the TE pin interrupt (ensures drawing is complete, avoids tearing).
