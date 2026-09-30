## Prototype hardware

Breadboard prototype to bring up each driver before the first PCB.
Current hardware requirements are in [HARDWARE_SELECTION.md](HARDWARE_SELECTION.md).

Goal: validate every peripheral (display, input, audio, SD) on the same MCU family as the final design.
Expect lower fps than final product - long jumper wires limit bus speeds, full speed is validated on the PCB.

### Dev board
- NUCLEO-H743ZI2
  - Same MCU family as the final design (STM32H743ZI, LQFP144)
  - Onboard ST-LINK - flashing, debugging and virtual COM port (UART logging) over one USB cable
  - All MCU pins on morpho headers - FMC, SDMMC, I2S, OCTOSPI all reachable
  - USB OTG connector - test DFU bootloader + USB later
  - User LEDs + user button - blinky / first bring up
  - Note: HSE comes from the ST-LINK MCO (8MHz), no crystal by default
  - Note: some pins are used by onboard Ethernet/LEDs - check conflicts in CubeMX (board selector)
- Everything runs at 3.3V - don't use 5V-only modules

### Display
- 320x240 ILI9341 or ST7789 module with 8-bit parallel (8080) interface
  - Common as 2.4"/2.8" "Arduino shield" style modules - check it is actually parallel and 3.3V capable
  - Check if the TE pin is broken out (often isn't) - without it, test without tear sync
- Start with a slow FMC timing (few MHz), speed up until errors appear
- Keep data/WR wires short and equal length
- Optional: a cheap SPI ST7789 module first - fewer wires for a quick first "pixels on screen"

### Controls
- Tactile push buttons on breadboard (12x12mm or 6x6mm)
  - 4 face (A,B,X,Y), 4 D-pad, 2 shoulder (L,R), 2 menu (Start, Select)
  - Internal pull-ups, software debounce - same as final design
- Analog thumbstick module (potentiometer based, 2 axis)
  - Same interface as the hall effect stick in the final design (2 ADC channels), much cheaper
  - Power it from 3.3V, not 5V (ADC max is 3.3V)

### Audio
- MAX98357A breakout board (e.g. Adafruit 3006) - same chip as final design
- Small speaker (4-8 ohm, ~1-3W)
- Shutdown (SD) pin wired to GPIO

### Storage
- microSD breakout that exposes all pins (DAT0-3, CMD, CLK, card detect)
  - Avoid cheap "Arduino SD modules" - most are SPI-only with 5V level shifters, SDMMC 4-bit won't work
- 10k-47k pull-ups on CMD, DAT0-3 if the breakout has none
- Keep SDMMC wires short, start at a low clock (e.g. 400kHz init, then a few MHz)

### Memory (optional)
- APS6404 PSRAM (SOIC-8) on a SOIC-8 to DIP adapter - only if external PSRAM is chosen

### Power
- Powered over the Nucleo's ST-LINK USB - no battery for the prototype
- Battery/charger/regulator circuit is validated on the PCB
  - Optional: charger + buck-boost breakout boards to test power path early

### Tools
- Breadboards (2-3 full size) + jumper wires (male-male, male-female)
- Multimeter
- Logic analyzer (e.g. cheap 8ch 24MHz + PulseView) - I2S, buttons, slow display/SD traffic
- Soldering iron - headers on breakouts, SOIC adapter

### Bring up order (matches roadmap)
- [ ] Blinky on Nucleo LED + UART logging over ST-LINK VCP
- [ ] Buttons + joystick (GPIO, ADC)
- [ ] Display (FMC 8080) - fill screen, then framebuffer + DMA
- [ ] Audio (I2S + DMA) - sine wave, then sample playback
- [ ] SD card (SDMMC + FatFS) - read a file, then load a game into RAM
- [ ] Measure: fps, current draw -> feeds the final power budget

### Shopping list
| Item                           | Qty | Notes                                |
|--------------------------------|-----|--------------------------------------|
| NUCLEO-H743ZI2                 | 1   |                                      |
| 320x240 8-bit parallel display | 1   | ILI9341/ST7789, 3.3V, TE pin if possible |
| Tactile buttons                | 12+ |                                      |
| Analog thumbstick module       | 1   |                                      |
| MAX98357A breakout             | 1   |                                      |
| Speaker 4-8 ohm                | 1   |                                      |
| microSD breakout (all pins)    | 1   | no level shifter, not SPI-only       |
| microSD card                   | 1   | FAT32, 4-32GB                        |
| Breadboards                    | 2-3 |                                      |
| Jumper wires                   | set |                                      |
| Logic analyzer                 | 1   | optional but very useful             |
| APS6404 + SOIC-8 adapter       | 1   | optional                             |
