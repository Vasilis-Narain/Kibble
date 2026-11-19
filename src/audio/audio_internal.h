/*
 * SPDX-FileCopyrightText: 2026 Vasilis Narain
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef __AUDIO_INTERNAL_H
#define __AUDIO_INTERNAL_H

#include <stdint.h>
#include <stdbool.h>

#include "audio.h"

#define AUDIO_MASTER_MAX_GAIN 128

#define AUDIO_SAMPLE_RATE 48000u
#define AUDIO_HALF_FRAMES 256u
#define AUDIO_HALF_SAMPLES (AUDIO_HALF_FRAMES * 2u)

#define AUDIO_MAX_VOICES 16u
#define AUDIO_MAX_MUSIC_VOICES 8u

enum {
    VOICE_ACTIVE = 1u << 0,
    VOICE_MUSIC = 1u << 1,
    VOICE_SFX = 1u << 2,
};

// SFX
#define AUDIO_SFX_POOL_SIZE 128000u
#define AUDIO_SFX_MAX_SAMPLE_SIZE 0xFFFFu
#define AUDIO_SFX_MAX_NUMBER 64u

typedef struct {
    const void *data;
    uint32_t len;
    int32_t volume;
    uint16_t sample_rate;
    uint8_t format;
    uint8_t max_instances;
} SfxDesc;

//audio.c
void audio_mix(int16_t *out);

//audio_voice.c
VoiceHandle audio_voice_sfx_alloc(Sfx sfx, const SfxDesc *desc);
void audio_voice_start(VoiceHandle voice, Sfx sfx, const void *data, uint32_t len, uint8_t format, uint16_t rate, int32_t volume, uint8_t kind);
void audio_voice_stop(VoiceHandle voice);
void audio_voice_resume(VoiceHandle voice);
void audio_voice_pause(VoiceHandle voice);
bool audio_voice_isplaying(VoiceHandle voice);
void audio_voice_mix_into(int32_t *acc, uint32_t frames);
void audio_voice_stop_many(uint32_t start, uint32_t end);

//audio_sfx.c
const SfxDesc *audio_sfx_get_desc(Sfx sfx);
void audio_sfx_set_volume(Sfx sfx, float volume);

#endif /* __AUDIO_INTERNAL_H */
