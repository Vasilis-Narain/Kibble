/*
 * SPDX-FileCopyrightText: 2026 Vasilis Narain
 *
 * SPDX-License-Identifier: MIT
 */
#ifndef KIBBLE_DISPLAY_H
#define KIBBLE_DISPLAY_H

#define SCREEN_WIDTH 320u
#define SCREEN_HEIGHT 240u

// display_hw.c
void display_init(void);
void display_set_brightness(float brightness);

#endif // KIBBLE_DISPLAY_H
