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
            int32_t x = (int32_t)(input_get_joy_axis_movement(AXIS_X) * 100);
            int32_t y = (int32_t)(input_get_joy_axis_movement(AXIS_Y) * 100);
            printf("%ld %ld\r\n", (long)x, (long)y);
        }

        while (HAL_GetTick() - tick < 17) {}
        tick += 17;
    }
}
