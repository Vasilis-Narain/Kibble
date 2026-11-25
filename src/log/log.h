/*
 * SPDX-FileCopyrightText: 2026 Vasilis Narain
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef KIBBLE_LOG_H
#define KIBBLE_LOG_H

#include <stdbool.h>

bool log_init(void);
void log_printf(const char *fmt, ...);
void log_printf_FromISR(const char *fmt, ...);

#ifdef DEBUG
#define LOG(fmt, ...) log_printf("%s:%d " fmt, __FILE__, __LINE__, ##__VA_ARGS__)
#else
#define LOG(...) ((void)0)
#endif // DEBUG

#endif /* KIBBLE_LOG_H */
