/* High-level input logic */
#include <stdint.h>

#include "input.h"

// flag holding the status of all button inputs
// NOTE(vasilis): the plan is to poll once per frame and update this flag
static uint32_t btn_inputs_status = 0;

uint32_t input_pack(uint32_t portb, uint32_t portf, uint32_t portd);
void input_update(uint32_t raw);
