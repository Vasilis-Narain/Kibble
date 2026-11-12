#include <stdint.h>
#include "input.h"

// flag holding the status of all button inputs
// NOTE(vasilis): the plan is for ISR's to set bits in this flag
// other subroutines/task will then use this and decide what to do
static volatile uint32_t btn_inputs_status = 0;
