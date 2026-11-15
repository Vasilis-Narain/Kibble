#ifndef __LOG_H
#define __LOG_H

#include <stdbool.h>

bool log_init(void);
void log_printf(const char *fmt, ...);
void log_printf_FromISR(const char *fmt, ...);

#endif /* __LOG_H */
