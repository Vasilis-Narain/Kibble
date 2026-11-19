/*
 * SPDX-FileCopyrightText: 2026 Vasilis Narain
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef TEST_DATA_H
#define TEST_DATA_H
#include <stdint.h>
// shared data
extern const int16_t big_clip[64000];
extern const uint32_t BIG_CLIP_LEN;

static const int16_t clip16[] = {100, -200, 300, -400};
static const uint32_t CLIP16_LEN = sizeof(clip16) / sizeof(clip16[0]);

static const int8_t clip8[] = {100, -50, 127, -100};
static const uint32_t CLIP8_LEN = sizeof(clip8) / sizeof(clip8[0]);

#endif
