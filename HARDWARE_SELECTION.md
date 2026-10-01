## Hardware requirements

Target: 320x240, 16 bit colour, 60 fps. Mainly 2D games, possibly DOOM style 3D games.
Games are copied onto an SD card and loaded into PSRAM at runtime.

### Overview
- MCU: STM32H7A3ZIT6 (280MHz, 2MB flash, ~1.4MB sram, LQFP144)
- PSRAM: APS6404L-3SQR-SN (8MB, OCTOSPI in quad mode)
- Display: 2.8" 320x240 IPS, ST7789V, 8-bit parallel (8080), with TE pin
- Audio: MAX98357AETE+T (I2S DAC + class-D amp) + 8 ohm 1W speaker
- Storage: microSD over SDMMC 4-bit
- Controls: 4 face + 4 D-pad + 2 shoulder + 2 menu + power button, hall effect joystick
- Power: 1S LiPo, USB-C charging, 3.3V for everything
- 1 green power LED

### MCU
- STM32H7A3 has OCTOSPI - needed to use PSRAM as normal RAM (STM32H743's QUADSPI can't write memory-mapped)
- LQFP144 - exposes FMC pins, hand-solderable
- Double buffered framebuffer (2x 150KB) in internal AXI sram
- Game code + assets in PSRAM
- 8MHz HSE crystal - accurate clocks for USB and audio
- Decoupling + VCAP capacitors - follow the datasheet reference design

### Display
- 8-bit parallel via FMC + DMA
  - 60 fps needs ~9.2MB/s (320 x 240 x 2 bytes x 60) - SPI can't reach this, parallel can
- TE pin to GPIO - start frame transfers on it to avoid tearing
- RESET pin to GPIO
- Backlight: N-MOSFET driven by timer PWM (brightness control)

### Controls
- Face buttons + D-pad: conductive rubber pads over gold plated (ENIG) PCB pads
- Shoulder + menu buttons: tactile switches
- Joystick: hall effect, 2 ADC channels
- All buttons on GPIO with internal pull-ups
- Debounce in software (poll once per frame) - no circuit needed

### Audio
- MAX98357A shutdown pin to GPIO - avoids pops, saves power
- Volume control in software
- No headphone jack in v1

### Storage
- microSD socket with card detect: Hirose DM3AT-SF-PEJM5
- Card detect to GPIO
- 47k pull-ups on CMD, DAT0-3

### Power
- 1S LiPo, 1500mAh, with protection circuit, JST-PH 2.0 connector
- Charger with power-path: BQ24072RGTR - device runs while charging
- USB-C receptacle: GCT USB4105-GF-A - 5.1k pull-downs on CC1/CC2
- 3.3V buck-boost: TPS63020DSJR - works across full LiPo range (3.0-4.2V)
- Battery voltage: resistor divider to ADC
- Power button + green power LED (GPIO)
- [ ] Current budget after prototype measurements -> confirm regulator and battery size

### Debug / programming
- SWD: 2x5 1.27mm header (SWDIO, SWCLK, NRST, SWO)
- USB + BOOT0 button - flash over built-in DFU bootloader
- UART pins for logging
- Reset button

### Pin budget
| Function        | Peripheral           | Pins |
|-----------------|----------------------|------|
| Display         | FMC 8080 (8-bit)     | 14   |
| Backlight       | TIM PWM              | 1    |
| SD card         | SDMMC 4-bit + detect | 7    |
| Audio           | I2S + shutdown       | 4    |
| Buttons         | GPIO                 | 13   |
| Power LED       | GPIO                 | 1    |
| Joystick        | ADC                  | 2    |
| Battery voltage | ADC                  | 1    |
| USB             | OTG FS               | 2    |
| Debug           | SWD + UART           | 4    |
| PSRAM           | OCTOSPI (quad)       | 6    |
| **Total**       |                      | **55** |
