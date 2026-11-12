/* High-level input logic file */
#include "input.h"

#include <stdint.h>
#include <stdbool.h>

// flag holding the status of all button inputs
// NOTE(vasilis): the plan is to poll once per frame and update this flag
static InputState input_state;

// doubles as a reset function.
void input_init(void) {
    input_state.curr = 0;
    input_state.last = 0;
    input_state.adc_x = 0;
    input_state.adc_y = 0;
}

void input_update(uint32_t raw) {
    input_state.last = input_state.curr;
    input_state.curr = raw;
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
