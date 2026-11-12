#ifndef __INPUT_H
#define __INPUT_H

#include <stdint.h>
#include <stdbool.h>
#include <assert.h>

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

    // joystick select (joystick press)
    INPUT_JOY_SEL,

    // power
    INPUT_POWER,

    // number of inputs
    INPUT_NUMBER,
} BtnInputFlagBit;

typedef struct {
    uint32_t curr;
    uint32_t last;
    int32_t adc_x;
    int32_t adc_y;
} InputState;

#define BTN_INPUT_MASK(bit_pos) (1u << (bit_pos))

static_assert(INPUT_NUMBER == 14, "InputFlagBit enum must have 14 inputs!");

// Application api
// NOTE(vasilis): call once per frame
void input_init(void);
void input_poll(void);

// Raylib style userland api
bool input_is_key_down(BtnInputFlagBit btn);
bool input_is_key_pressed(BtnInputFlagBit btn);
bool input_is_key_up(BtnInputFlagBit btn);
bool input_is_key_released(BtnInputFlagBit btn);
bool input_is_key_held(BtnInputFlagBit btn);

// Internal use
void input_update(uint32_t raw);

#endif /* __INPUT_H */
