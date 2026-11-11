#include "app.h"
#include "main.h"

void app_main(void) {
    for (;;) {
        HAL_GPIO_TogglePin(PWR_LED_GPIO_Port, PWR_LED_Pin);
        HAL_Delay(500);
    }
}
