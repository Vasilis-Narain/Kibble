/* Low_level input logic file */
#include "input.h"

#include <stdint.h>

// cubeMX driver includes
#include "main.h"
#include "adc.h"

#define JOY_DMA_BUF_LEN 2u

static ADC_HandleTypeDef *const joy_adc = &hadc1;
__attribute__((section(".RAM_NOCACHE"), aligned(32))) static volatile uint16_t joy_dma_buf[JOY_DMA_BUF_LEN];

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
void input_poll(void) {
    uint32_t raw = input_read_raw();
    input_update(raw, joy_dma_buf[0], joy_dma_buf[1]);
}

//NOTE(vasilis): in the future this function will also be used to provide
//userland joystick calibration. This is needed in case the device is started with the joystick not centred.
#define CENTRE_SAMPLES_SHIFT 4u
void input_calculate_centres(bool recalibrate) {
    uint32_t tick = HAL_GetTick();

    // If we're in init phase: short spinloop to make sure adc has done a couple conversions first
    if (!recalibrate) {
        while (HAL_GetTick() - tick < 20) {}
        tick += 20;
    }

    // shouldnt overflow (worst case 4095 * 16 = 0xFFF0 < UINT16 max)
    uint16_t centre_x = 0;
    uint16_t centre_y = 0;
    for (size_t i = 0; i < (1u << CENTRE_SAMPLES_SHIFT); ++i) { // take 16 samples (avg is a bit shift), at 10ms tick per sample this procedure takes 160 ms
        centre_x += joy_dma_buf[0];
        centre_y += joy_dma_buf[1];

        while (HAL_GetTick() - tick < 10) {}
        tick += 10;
    }
    input_adc_register_centres(centre_x >> CENTRE_SAMPLES_SHIFT, centre_y >> CENTRE_SAMPLES_SHIFT);
}

void input_joystick_init(void) {
    // NOTE(vasilis): .RAM_NOCACHE is NOLOAD so we need to init
    // 12 bit adc, max value 0..4095, midpoint is ~2048
    joy_dma_buf[0] = 2048;
    joy_dma_buf[1] = 2048;

    if (HAL_ADCEx_Calibration_Start(joy_adc, ADC_CALIB_OFFSET_LINEARITY, ADC_SINGLE_ENDED) != HAL_OK) {
        Error_Handler();
    }

    if (HAL_ADC_Start_DMA(joy_adc, (uint32_t *)joy_dma_buf, JOY_DMA_BUF_LEN) != HAL_OK) {
        Error_Handler();
    }

    input_calculate_centres(false);
}
