/* High-level input logic file */
#include "input.h"

#include <stdint.h>
#include <stdbool.h>
#include <assert.h>

// flag holding the status of all button inputs
// NOTE(vasilis): the plan is to poll once per frame and update this flag
static InputState input_state;

void input_update(uint32_t raw_btns, uint32_t raw_adc_x, uint32_t raw_adc_y) {
    input_state.last = input_state.curr;
    input_state.curr = raw_btns;
    input_state.adc_x_raw = raw_adc_x;
    input_state.adc_y_raw = raw_adc_y;
}

// NOTE(vasilis): tmp testing function
uint16_t input_state_adc_raw(uint32_t axis) {
    assert(axis < 2);
    if (axis == 0) {
        return input_state.adc_x_raw;
    }
    return input_state.adc_y_raw;
}

static bool input_was_key_pressed(BtnInputFlagBit btn) {
    return input_state.last & BTN_INPUT_MASK(btn);
}

bool input_is_key_down(BtnInputFlagBit btn) {
    return input_state.curr & BTN_INPUT_MASK(btn);
}

bool input_is_key_pressed(BtnInputFlagBit btn) {
    return input_is_key_down(btn) && !input_was_key_pressed(btn);
}

bool input_is_key_up(BtnInputFlagBit btn) {
    return !input_is_key_down(btn);
}

bool input_is_key_released(BtnInputFlagBit btn) {
    return input_is_key_up(btn) && input_was_key_pressed(btn);
}

bool input_is_key_held(BtnInputFlagBit btn) {
    return input_is_key_down(btn) && input_was_key_pressed(btn);
}
