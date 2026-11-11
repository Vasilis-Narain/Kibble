## Prototype hardware

Breadboard prototype to bring up each driver before the first PCB.
Final hardware is in [HARDWARE_SELECTION.md](HARDWARE_SELECTION.md).
Expect lower fps than the final product.

### Notes
- NUCLEO-H7A3ZI-Q: same MCU as the final design, onboard ST-LINK (flash, debug, UART over USB)
  - "-Q" = SMPS supply - power config in CubeMX must match the board
  - Pins used by the board ([UM2408](https://www.st.com/resource/en/user_manual/um2408-stm32h7-nucleo144-boards-mb1363-stmicroelectronics.pdf)): LEDs PB0/PE1/PB14, button PC13, VCP PD8/PD9, USB PA9-PA12 + PG7,
    SWD PA13/PA14, SWO PB3, HSE PH0 (8MHz MCO from ST-LINK), LSE PC14/PC15 (X3 crystal)
  - To avoid D13/D14 PD8/PD9 conflict with Nucleo's ST-LINK the Prototype will use 8 bit parallel mode.
- All signals at 3.3V - power the joystick from 3.3V (ADC max is 3.3V)
  - Only exception: display backlight anode from the Nucleo 5V pin (LEDs need 3.1V + headroom)
- Display: same panel as the final design, on an NHD-FFC40 (passive 1:1 FFC to 2x20 2.54mm)
  - Not the NHD-TFT40 - it's for RGB displays and puts 5V/LED driver on our signal pins
  - IM0, IM1, IM2 to GND (16-bit), VDD + VDDI to 3.3V
  - 2x20 header doesn't fit a breadboard - wire with female-male jumpers
  - Backlight: 5V -> 33 ohm -> LED-A, LED-K1-K4 tied -> PN2222A collector, emitter to GND,
    base via 1k to a timer PWM pin (~60mA, about 40% brightness)
- SD breakout must expose all SDIO pins - most "Arduino SD modules" are SPI-only and won't work
- Start display and SD at low clocks and keep wires short

### Bring up order
- Blinky + logging (UART VCP until the display rework, then SWO)
- Buttons + joystick
- Display - fill screen, then framebuffer + DMA
- Audio - sine wave, then sample playback
- SD card - read a file, then load a game into sram and run it
- Measure fps + current draw

### Shopping list
| Item                    | Part                                         | Qty |
|-------------------------|----------------------------------------------|-----|
| Dev board               | [ST NUCLEO-H7A3ZI-Q](https://www.digikey.it/en/products/detail/stmicroelectronics/NUCLEO-H7A3ZI-Q/11482046) | 1   |
| Display                 | [NHD-2.8-240320AF-CSXP-F](https://www.digikey.it/en/products/detail/newhaven-display-intl/NHD-2-8-240320AF-CSXP-F/9849907) | 1   |
| FFC breakout            | [NHD-FFC40](https://www.digikey.it/en/products/detail/newhaven-display-intl/NHD-FFC40/2165899) | 1   |
| Backlight transistor    | [onsemi PN2222ABU](https://www.digikey.it/en/products/detail/onsemi/PN2222ABU/6534) | 2   |
| Backlight resistor      | [Stackpole CF14JT33R0 (33 ohm 1/4W)](https://www.digikey.it/en/products/detail/stackpole-electronics-inc/CF14JT33R0/1741397) | 2   |
| Transistor base resistor | [Stackpole CF14JT1K00 (1k 1/4W)](https://www.digikey.it/en/products/detail/stackpole-electronics-inc/CF14JT1K00/1741314) | 2   |
| Audio amp               | [Adafruit 3006 (MAX98357A breakout)](https://www.digikey.it/en/products/detail/adafruit-industries-llc/3006/6058477?s=N4IgTCBcDaIIYBM4DMBOBXAlgFwAQGYAGQgNhAF0BfIA)           | 1   |
| Speaker                 | [Adafruit 3351 (4 ohm 3W)](https://www.digikey.it/en/products/detail/adafruit-industries-llc/3351/6612456?s=N4IgTCBcDaIIIBMCGAzATgVwJYBcAEAzAQKwCMIAugL5A)                     | 1   |
| microSD breakout        | [Adafruit 4682 (SPI/SDIO, 3V)](https://www.digikey.it/en/products/detail/adafruit-industries-llc/4682/12822319?s=N4IgTCBcDaIIIBMCGAzATgVwJYBcAEALAGwAcEAugL5A)                 | 1   |
| microSD card            | 4-32GB, FAT32                                | 1   |
| Joystick                | [Adafruit 512 (thumb joystick + breakout)](https://www.digikey.it/en/products/detail/512/1528-2124-ND/7056915) | 1   |
| Tactile buttons         | [Adafruit 1119 (12mm, 10 pack)](https://www.digikey.it/en/products/detail/adafruit-industries-llc/1119/7241449) | 2   |
| Green LED + 330 ohm     | 5mm                                          | 1   |
| Breadboards             | 830 points                                   | 3   |
| Jumper wires            | male-male + male-female set                  | 1   |
| Logic analyzer          | 8ch 24MHz (PulseView compatible)             | 1   |
