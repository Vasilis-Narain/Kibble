/* High-level input logic file */
#include "input.h"

#include <stdint.h>
#include <stdbool.h>
#include <assert.h>

// flag holding the status of all button inputs
static InputState input_state;

// store calculated centres at rest after averaging a few samples
static uint16_t centre_x;
static uint16_t centre_y;

static float input_axis_normalise(uint16_t raw, uint16_t centre);

void input_update(uint32_t raw_btns, uint16_t raw_adc_x, uint16_t raw_adc_y) {
    input_state.last = input_state.curr;
    input_state.curr = raw_btns;

    input_state.joy_x = input_axis_normalise(raw_adc_x, centre_x);
    input_state.joy_y = input_axis_normalise(raw_adc_y, centre_y);
}

void input_adc_register_centres(uint16_t x, uint16_t y) {
    centre_x = x;
    centre_y = y;
}

float input_get_joy_axis_movement(JoyAxis axis) {
    switch (axis) {
    case AXIS_X:
        return input_state.joy_x;
    case AXIS_Y:
        return input_state.joy_y;
    default:
        return 0.0f;
    }
}

enum {
    // TODO(vasilis): eventually deadzones will be configurable
    ADC_DEADZONE = 100,
    ADC_OUTER_DEADZONE = 50,
    ADC_MAX = 0xFFF - ADC_OUTER_DEADZONE,
    ADC_MIN = 0 + ADC_OUTER_DEADZONE,
};

static float input_axis_normalise(uint16_t raw, uint16_t centre) {
    int32_t d = (int32_t)raw - (int32_t)centre;
    int32_t range = (d >= 0) ? (ADC_MAX - centre) : (centre - ADC_MIN);
    int32_t mag = (d >= 0) ? d : -d;

    if (mag <= ADC_DEADZONE) {
        return 0.0f;
    }
    if (mag > range) {
        mag = range;
    }

    float out = (float)(mag - ADC_DEADZONE) / (float)(range - ADC_DEADZONE);
    return (d >= 0) ? out : -out;
}

static bool input_was_key_down(BtnInputFlagBit btn) {
    return input_state.last & BTN_INPUT_MASK(btn);
}

bool input_is_key_down(BtnInputFlagBit btn) {
    return input_state.curr & BTN_INPUT_MASK(btn);
}

bool input_is_key_pressed(BtnInputFlagBit btn) {
    return input_is_key_down(btn) && !input_was_key_down(btn);
}

bool input_is_key_up(BtnInputFlagBit btn) {
    return !input_is_key_down(btn);
}

bool input_is_key_released(BtnInputFlagBit btn) {
    return input_is_key_up(btn) && input_was_key_down(btn);
}

bool input_is_key_held(BtnInputFlagBit btn) {
    return input_is_key_down(btn) && input_was_key_down(btn);
}
