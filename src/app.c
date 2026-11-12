#include "app.h"

#include <stdio.h>

#include "main.h"
#include "input.h"

void app_main(void) {
    input_joystick_init();

    uint32_t tick = HAL_GetTick();
    static uint32_t frame = 0;
    for (;;) {
        input_poll();

        if (input_is_key_pressed(INPUT_A)) {
            printf("Input A is pressed\r\n");
        }
        if (input_is_key_pressed(INPUT_JOY_SEL)) {
            printf("Input JOY_SEL is pressed\r\n");
        }

        if (++frame % 30 == 0) {
            HAL_GPIO_TogglePin(PWR_LED_GPIO_Port, PWR_LED_Pin);
            printf("%u %u\r\n", input_state_adc_raw(0), input_state_adc_raw(1));
        }

        while (HAL_GetTick() - tick < 17) {}
        tick += 17;
    }
}
