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

typedef enum {
    AXIS_X = 0,
    AXIS_Y,
    AXIS_NUMBER,
} JoyAxis;

typedef struct {
    uint32_t curr;
    uint32_t last;

    // x, y take values between -1 and 1
    float joy_x;
    float joy_y;
} InputState;

#define BTN_INPUT_MASK(bit_pos) (1u << (bit_pos))

static_assert(INPUT_NUMBER == 14, "InputFlagBit enum must have 14 inputs!");

// Application api
void input_joystick_init(void);
void input_poll(void);
void input_adc_register_centres(uint16_t x, uint16_t y);
void input_calculate_centres(bool recalibrate);

// tmp function for testing
uint16_t input_state_adc_raw(uint32_t axis);

// Raylib style userland api
bool input_is_key_down(BtnInputFlagBit btn);
bool input_is_key_pressed(BtnInputFlagBit btn);
bool input_is_key_up(BtnInputFlagBit btn);
bool input_is_key_released(BtnInputFlagBit btn);
bool input_is_key_held(BtnInputFlagBit btn);
float input_get_joy_axis_movement(JoyAxis axis);

// Internal use
void input_update(uint32_t raw_btns, uint16_t raw_adc_x, uint16_t raw_adc_y);

#endif /* __INPUT_H */
