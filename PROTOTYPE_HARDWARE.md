## Prototype hardware

Breadboard prototype to bring up each driver before the first PCB.
Final hardware is in [HARDWARE_SELECTION.md](HARDWARE_SELECTION.md).
Expect lower fps than the final product.

### Notes
- NUCLEO-H7A3ZI-Q: same MCU as the final design, onboard ST-LINK (flash, debug, UART over USB)
  - "-Q" = SMPS supply - power config in CubeMX must match the board
  - Check pin conflicts with onboard LEDs/USB in CubeMX
- Everything at 3.3V - power the joystick from 3.3V (ADC max is 3.3V)
- Display must be 8-bit parallel and 3.3V capable, TE pin broken out if possible
- SD breakout must expose all SDIO pins - most "Arduino SD modules" are SPI-only and won't work
- Start display, SD and PSRAM at low clocks and keep wires short

### Bring up order
- [ ] Blinky + UART logging
- [ ] Buttons + joystick
- [ ] Display - fill screen, then framebuffer + DMA
- [ ] Audio - sine wave, then sample playback
- [ ] PSRAM - read/write test, then run code from it
- [ ] SD card - read a file, then load a game into PSRAM
- [ ] Measure fps + current draw

### Shopping list
| Item                    | Part                                         | Qty |
|-------------------------|----------------------------------------------|-----|
| Dev board               | ST NUCLEO-H7A3ZI-Q                           | 1   |
| Display                 | 2.8" 320x240 ST7789V/ILI9341, 8-bit parallel | 1   |
| Audio amp               | Adafruit 3006 (MAX98357A breakout)           | 1   |
| Speaker                 | Adafruit 3351 (4 ohm 3W)                     | 1   |
| microSD breakout        | Adafruit 4682 (SPI/SDIO, 3V)                 | 1   |
| microSD card            | 4-32GB, FAT32                                | 1   |
| PSRAM                   | APS6404L-3SQR-SN                             | 2   |
| SOIC-8 to DIP adapter   | any                                          | 2   |
| Joystick                | KY-023 module                                | 1   |
| Tactile buttons         | 6x6mm through-hole                           | 15  |
| Green LED + 330 ohm     | 5mm                                          | 1   |
| Breadboards             | 830 points                                   | 3   |
| Jumper wires            | male-male + male-female set                  | 1   |
| Logic analyzer          | 8ch 24MHz (PulseView compatible)             | 1   |
