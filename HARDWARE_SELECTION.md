## Hardware requirements

Target: 320x240 display at 60 fps. Mainly 2D games, possibly DOOM style 3D games.
Games are copied onto an SD card and loaded into RAM at runtime.

### Open decisions
- [ ] Display bus: 8-bit or 16-bit parallel (8080) - SPI can't reach 60 fps (see Display)
- [ ] External PSRAM: yes / leave pins free - decided by whether DOOM style games are a goal
- [ ] Colour depth: 16-bit (RGB565) or 8-bit palette (halves RAM + bandwidth, GBA-like)
- [ ] D-pad, joystick, or both
- [ ] Exact MCU part number

### MCU
- STM32H7 series @480MHz (e.g. STM32H743ZI - 2MB flash, 1MB sram)
- LQFP144 package - exposes FMC pins, hand-solderable (avoid BGA)
- Internal sram is split in banks (512KB AXI, 288KB SRAM1-3, 128KB DTCM, 64KB SRAM4)
  - Double buffered framebuffer (2x 150KB) fits in AXI sram
  - Remaining ram is shared by game code + assets loaded from SD
- Optional external PSRAM over OCTOSPI (e.g. APS6404, 8MB) - likely needed for DOOM style games
- HSE crystal - accurate clocks for USB and audio sample rates
- LSE 32.768kHz crystal (optional) - RTC for save timestamps
- Decoupling, VCAP capacitors and SMPS/LDO config - follow reference design for the exact part

### Display
- 8-bit parallel (8080) ips display, 320x240
- Framebuffer: 320 x 240 x 2 bytes (RGB565) = 153.6KB
- Bandwidth @60 fps: 153.6KB x 60 = ~9.2MB/s (~74Mbit/s)
  - SPI: ST7789 max ~62.5MHz -> ~50 fps at best, ILI9341 spec is 10MHz -> not enough
  - 8-bit parallel via FMC + DMA: ~15MHz write strobe -> ~15MB/s -> ~97 fps
- TE (tearing effect) pin to GPIO - sync frame transfers to avoid tearing
- RESET pin to GPIO
- Backlight: MOSFET driven by timer PWM (brightness control)

### Controls
- 4 face buttons (A,B,X,Y) - conductive silicone rubber pads over PCB traces
  - Gold plated (ENIG) pads - bare copper oxidises
- D-pad (4 buttons) - same rubber pad construction
- 2 shoulder buttons (L,R) - tactile switches
- 2 menu buttons (Start, Select) - simple tactile switches
- Power button
- 1 analog joystick for movement - hall effect (no drift), 2 ADC channels
- All buttons on GPIO with internal pull-ups
- Debounce: in software, no circuit needed
  - Poll once per frame (16.7ms > typical bounce time) or 1kHz timer with 2-3 sample filter

### Audio
- I2S DAC + class-D amp - MAX98357A (mono, no separate DAC needed)
- Speaker - 8 ohm, ~1W
- Amp shutdown/mute pin to GPIO - avoids pops, saves power
- Volume control in software
- Headphone jack - not for v1 (needs separate DAC + headphone amp)

### Storage
- microSD card over SDMMC 4-bit (much faster than SPI)
- Card detect switch to GPIO
- Pull-ups on CMD, DAT0-3 (10k-47k)
- FatFS on the firmware side

### Power
- 1 cell LiPo battery, 1000-2000mAh
- Charger IC with power-path (e.g. BQ24072) - device runs while charging
- USB-C connector - 5.1k pull-downs on CC1/CC2 (required for chargers to supply power)
- 3.3V buck-boost regulator (e.g. TPS63020) - LiPo range 3.0-4.2V, a plain LDO drops out near empty
- Battery voltage monitoring - resistor divider to ADC channel
- Power switch or soft power button
- [ ] Current budget: MCU @480MHz + backlight + amp peaks -> size regulator and battery

### Debug / programming
- SWD header (+ NRST) - Tag-Connect or 2x5 1.27mm
- USB (OTG FS) + BOOT0 button - flash via built-in DFU bootloader without a debugger
- UART pins exposed for logging
- Reset button

### Peripheral / pin budget
| Function        | Peripheral               | Pins (approx.) |
|-----------------|--------------------------|----------------|
| Display         | FMC 8080 (8-bit)         | ~14            |
| Backlight       | TIM PWM                  | 1              |
| SD card         | SDMMC 4-bit + detect     | 7              |
| Audio           | I2S + shutdown           | 4              |
| Buttons         | GPIO                     | ~13            |
| Joystick        | ADC                      | 2              |
| Battery voltage | ADC                      | 1              |
| USB             | OTG FS                   | 2              |
| Debug           | SWD + UART               | 4              |
| PSRAM (opt.)    | OCTOSPI                  | 6              |
| **Total**       |                          | **~55**        |

- [ ] Check pin mapping in STM32CubeMX - FMC and SDMMC pins are fixed
