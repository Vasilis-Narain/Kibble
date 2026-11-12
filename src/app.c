#include "app.h"

#include <stdio.h>

#include "main.h"
#include "input.h"

void app_main(void) {

    uint32_t tick = HAL_GetTick();
    for (;;) {
        HAL_GPIO_TogglePin(PWR_LED_GPIO_Port, PWR_LED_Pin);
        input_poll();
        if (input_is_key_down(INPUT_A)) {
            printf("Input A is down\n");
        }
        while (HAL_GetTick() - tick < 17) {}
    }
}
