/* High-level input logic file */
#include "input.h"

#include <stdint.h>
#include <stdbool.h>

// flag holding the status of all button inputs
// NOTE(vasilis): the plan is to poll once per frame and update this flag
static uint32_t btn_inputs_status = 0;
static uint32_t btn_inputs_status_last_frame = 0;

void input_update(uint32_t raw) {
    btn_inputs_status_last_frame = btn_inputs_status;
    btn_inputs_status = raw;
}

static bool input_was_key_pressed(BtnInputFlagBit btn) {
    return btn_inputs_status_last_frame & BTN_INPUT_MASK(btn);
}

bool input_is_key_down(BtnInputFlagBit btn) {
    return btn_inputs_status & BTN_INPUT_MASK(btn);
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
