/*
 * SPDX-FileCopyrightText: 2026 Vasilis Narain
 *
 * SPDX-License-Identifier: MIT
 */
#ifndef KIBBLE_DISPLAY_COMMANDS_H
#define KIBBLE_DISPLAY_COMMANDS_H

// ST7789VI command set, transcribed from the Sitronix ST7789VI datasheet v1.6 (2020/01):
// https://support.newhavendisplay.com/hc/en-us/article_attachments/28274702816407
// Names and values follow the detailed command sections (9.1.x / 9.2.x). The summary
// command table in the same datasheet uses some different names and has a copy-paste
// errors (eg GAMSET row)

typedef enum {
    // 9.1 System function commands
    DCMD_NOP = 0x00,       // no operation
    DCMD_SWRESET = 0x01,   // software reset
    DCMD_RDDID = 0x04,     // read display id
    DCMD_RDDST = 0x09,     // read display status
    DCMD_RDDPM = 0x0a,     // read display power mode
    DCMD_RDDMADCTL = 0x0b, // read display madctl
    DCMD_RDDCOLMOD = 0x0c, // read display pixel format
    DCMD_RDDIM = 0x0d,     // read display image mode
    DCMD_RDDSM = 0x0e,     // read display signal mode
    DCMD_RDDSDR = 0x0f,    // read display self-diagnostic result
    DCMD_SLPIN = 0x10,     // sleep in
    DCMD_SLPOUT = 0x11,    // sleep out
    DCMD_PTLON = 0x12,     // partial display mode on
    DCMD_NORON = 0x13,     // normal display mode on
    DCMD_INVOFF = 0x20,    // display inversion off
    DCMD_INVON = 0x21,     // display inversion on
    DCMD_GAMSET = 0x26,    // gamma set
    DCMD_DISPOFF = 0x28,   // display off
    DCMD_DISPON = 0x29,    // display on
    DCMD_CASET = 0x2a,     // column address set
    DCMD_RASET = 0x2b,     // row address set
    DCMD_RAMWR = 0x2c,     // memory write
    DCMD_RAMRD = 0x2e,     // memory read
    DCMD_PTLAR = 0x30,     // partial area
    DCMD_VSCRDEF = 0x33,   // vertical scrolling definition
    DCMD_TEOFF = 0x34,     // tearing effect line off
    DCMD_TEON = 0x35,      // tearing effect line on
    DCMD_MADCTL = 0x36,    // memory data access control
    DCMD_VSCSAD = 0x37,    // vertical scroll start address of ram
    DCMD_IDMOFF = 0x38,    // idle mode off
    DCMD_IDMON = 0x39,     // idle mode on
    DCMD_COLMOD = 0x3a,    // interface pixel format
    DCMD_WRMEMC = 0x3c,    // write memory continue
    DCMD_RDMEMC = 0x3e,    // read memory continue
    DCMD_STE = 0x44,       // set tear scanline
    DCMD_GSCAN = 0x45,     // get scanline
    DCMD_WRDISBV = 0x51,   // write display brightness
    DCMD_RDDISBV = 0x52,   // read display brightness value
    DCMD_WRCTRLD = 0x53,   // write ctrl display
    DCMD_RDCTRLD = 0x54,   // read ctrl value display
    DCMD_WRCACE = 0x55,    // write content adaptive brightness control and color enhancement
    DCMD_RDCABC = 0x56,    // read content adaptive brightness control
    DCMD_WRCABCMB = 0x5e,  // write cabc minimum brightness
    DCMD_RDCABCMB = 0x5f,  // read cabc minimum brightness
    DCMD_RDABCSDR = 0x68,  // read automatic brightness control self-diagnostic result
    DCMD_RDID1 = 0xda,     // read id1
    DCMD_RDID2 = 0xdb,     // read id2
    DCMD_RDID3 = 0xdc,     // read id3

    // 9.2 System function command table 2
    DCMD_RAMCTRL = 0xb0,   // ram control
    DCMD_RGBCTRL = 0xb1,   // rgb interface control
    DCMD_PORCTRL = 0xb2,   // porch setting
    DCMD_FRCTRL1 = 0xb3,   // frame rate control 1 (in partial mode / idle colors)
    DCMD_PARCTRL = 0xb5,   // partial mode control
    DCMD_GCTRL = 0xb7,     // gate control
    DCMD_GTADJ = 0xb8,     // gate on timing adjustment
    DCMD_DGMEN = 0xba,     // digital gamma enable
    DCMD_VCOMS = 0xbb,     // vcoms setting
    DCMD_LCMCTRL = 0xc0,   // lcm control
    DCMD_IDSET = 0xc1,     // id code setting
    DCMD_VDVVRHEN = 0xc2,  // vdv and vrh command enable
    DCMD_VRHS = 0xc3,      // vrh set
    DCMD_VDVS = 0xc4,      // vdv set
    DCMD_VCMOFSET = 0xc5,  // vcoms offset set
    DCMD_FRCTRL2 = 0xc6,   // frame rate control in normal mode
    DCMD_CABCCTRL = 0xc7,  // cabc control
    DCMD_REGSEL1 = 0xc8,   // register value selection 1
    DCMD_REGSEL2 = 0xca,   // register value selection 2
    DCMD_PWMFRSEL = 0xcc,  // pwm frequency selection
    DCMD_PWCTRL1 = 0xd0,   // power control 1
    DCMD_VAPVANEN = 0xd2,  // enable vap/van signal output
    DCMD_GATESEL = 0xd6,   // gate output selection in sleep in mode
    DCMD_CMD2EN = 0xdf,    // command 2 enable
    DCMD_PVGAMCTRL = 0xe0, // positive voltage gamma control
    DCMD_NVGAMCTRL = 0xe1, // negative voltage gamma control
    DCMD_DGMLUTR = 0xe2,   // digital gamma look-up table for red
    DCMD_DGMLUTB = 0xe3,   // digital gamma look-up table for blue
    DCMD_GATECTRL = 0xe4,  // gate control
    DCMD_SPI2EN = 0xe7,    // spi2 enable
    DCMD_PWCTRL2 = 0xe8,   // power control 2
    DCMD_EQCTRL = 0xe9,    // equalize time control
    DCMD_PROMCTRL = 0xec,  // program mode control
    DCMD_PROMEN = 0xfa,    // program mode enable
    DCMD_NVMSET = 0xfc,    // nvm setting
    DCMD_PROMACT = 0xfe,   // program action
} ST7789VI_Cmd;

#endif // KIBBLE_DISPLAY_COMMANDS_H
