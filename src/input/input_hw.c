/* Low_level input logic file */
#include "input.h"

#include <stdint.h>

// cubeMX driver includes
#include "main.h"
//#include "adc.h"

typedef struct {
    GPIO_TypeDef *port;
    uint16_t pin;
} BtnPin;

// Table to map port/pin to application flags
// index = BtnInputFlagBit.
static const BtnPin btn_pins[INPUT_NUMBER] = {
    //letters
    [INPUT_A] = {BTN_A_GPIO_Port, BTN_A_Pin},
    [INPUT_B] = {BTN_B_GPIO_Port, BTN_B_Pin},
    [INPUT_X] = {BTN_X_GPIO_Port, BTN_X_Pin},
    [INPUT_Y] = {BTN_Y_GPIO_Port, BTN_Y_Pin},

    //dpad
    [INPUT_UP] = {BTN_UP_GPIO_Port, BTN_UP_Pin},
    [INPUT_DOWN] = {BTN_DOWN_GPIO_Port, BTN_DOWN_Pin},
    [INPUT_LEFT] = {BTN_LEFT_GPIO_Port, BTN_LEFT_Pin},
    [INPUT_RIGHT] = {BTN_RIGHT_GPIO_Port, BTN_RIGHT_Pin},

    //bumpers
    [INPUT_LB] = {BTN_L_GPIO_Port, BTN_L_Pin},
    [INPUT_RB] = {BTN_R_GPIO_Port, BTN_R_Pin},

    //options
    [INPUT_START] = {BTN_START_GPIO_Port, BTN_START_Pin},
    [INPUT_SELECT] = {BTN_SELECT_GPIO_Port, BTN_SELECT_Pin},

    //joystick press
    [INPUT_JOY_SEL] = {JOY_SEL_GPIO_Port, JOY_SEL_Pin},

    //pwr
    [INPUT_POWER] = {BTN_POWER_GPIO_Port, BTN_POWER_Pin},
};

// Takes the LL info scattered across different ports and pins with arbitrary numbers,
// and outputs it into a single u32. This setup should survive any pin changes in cubeMX
// TODO(vasilis): test on device still needed
static uint32_t input_read_raw(void) {
    uint32_t raw = 0;

    for (uint32_t i = 0; i < INPUT_NUMBER; ++i) {
        if (!(btn_pins[i].port->IDR & btn_pins[i].pin)) {
            raw |= BTN_INPUT_MASK(i);
        }
    }

    return raw;
}

// Application api: call once per frame
// TODO(vasilis): this will also read the ADC numbers once that's done
void input_poll(void) {
    uint32_t raw = input_read_raw();
    input_update(raw);
}
