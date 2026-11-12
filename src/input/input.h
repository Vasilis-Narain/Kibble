#ifndef __INPUT_H
#define __INPUT_H

#include <stdint.h>

typedef enum {
    INPUT_A = 0,
    INPUT_X,
    INPUT_Y,
    INPUT_B,

    // dpad
    INPUT_DOWN,
    INPUT_LEFT,
    INPUT_UP,
    INPUT_RIGHT,

    // bumpers
    INPUT_LB,
    INPUT_RB,

    // start/select
    INPUT_START,
    INPUT_SELECT,

    // number of inputs
    INPUT_NUMBER,
} BtnInputFlagBit;

#define BTN_INPUT_MASK(bit_pos) (1u << bit_pos)

_Static_assert(INPUT_NUMBER == 12, "InputFlagBit enum must have 12 inputs!");

uint32_t input_pack(uint32_t portb, uint32_t portf, uint32_t portd);
void input_update(uint32_t raw);

#endif /* __INPUT_H */
