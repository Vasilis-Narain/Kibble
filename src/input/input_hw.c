/* Low_level input logic file */
#include "input.h"

#include <stdint.h>

// cubeMX driver includes
#include "main.h"
//#include "adc.h"

static uint32_t input_read_raw(void) {
    uint32_t portB = GPIOB->IDR;
    uint32_t portF = GPIOF->IDR;
    uint32_t portD = GPIOD->IDR;

    uint32_t raw = 0;

    // port b
    if (!(portB & BTN_UP_Pin)) {
        raw |= BTN_INPUT_MASK(INPUT_UP);
    }
    if (!(portB & BTN_DOWN_Pin)) {
        raw |= BTN_INPUT_MASK(INPUT_DOWN);
    }
    if (!(portB & BTN_LEFT_Pin)) {
        raw |= BTN_INPUT_MASK(INPUT_LEFT);
    }
    if (!(portB & BTN_RIGHT_Pin)) {
        raw |= BTN_INPUT_MASK(INPUT_RIGHT);
    }
    if (!(portB & BTN_A_Pin)) {
        raw |= BTN_INPUT_MASK(INPUT_A);
    }
    if (!(portB & BTN_B_Pin)) {
        raw |= BTN_INPUT_MASK(INPUT_B);
    }
    if (!(portB & BTN_X_Pin)) {
        raw |= BTN_INPUT_MASK(INPUT_X);
    }

    // port d
    if (!(portD & BTN_POWER_Pin)) {
        raw |= BTN_INPUT_MASK(INPUT_POWER);
    }
    if (!(portD & JOY_SEL_Pin)) {
        raw |= BTN_INPUT_MASK(INPUT_JOY_SEL);
    }

    // port f
    if (!(portF & BTN_Y_Pin)) {
        raw |= BTN_INPUT_MASK(INPUT_Y);
    }
    if (!(portF & BTN_L_Pin)) {
        raw |= BTN_INPUT_MASK(INPUT_LB);
    }
    if (!(portF & BTN_R_Pin)) {
        raw |= BTN_INPUT_MASK(INPUT_RB);
    }
    if (!(portF & BTN_START_Pin)) {
        raw |= BTN_INPUT_MASK(INPUT_START);
    }
    if (!(portF & BTN_SELECT_Pin)) {
        raw |= BTN_INPUT_MASK(INPUT_SELECT);
    }

    return raw;
}

void input_poll(void) {
    uint32_t raw = input_read_raw();
    input_update(raw);
}
