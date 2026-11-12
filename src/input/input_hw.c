/* low_level input logic */
#include <stdint.h>

#include "input.h"
#include "stm32h7a3xxq.h"

void input_poll(void) {
    uint32_t raw = input_pack(GPIOB->IDR, GPIOF->IDR, GPIOD->IDR);
    input_update(raw);
}
