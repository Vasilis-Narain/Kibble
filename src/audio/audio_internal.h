#ifndef __AUDIO_INTERNAL_H
#define __AUDIO_INTERNAL_H

#include <stdint.h>

#include "audio.h"

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
#define AUDIO_SFX_MAX_NUMBER 64u

typedef struct {
    const void *data;
    uint32_t len;
    uint16_t sample_rate;
    uint8_t format;
} SfxDesc;

//audio.c
void audio_mix(int16_t *out);

//audio_voice.c
VoiceHandle audio_voice_alloc(uint32_t first, uint32_t last);
void audio_voice_start(VoiceHandle voice, const void *data, uint32_t len, uint8_t format, uint16_t rate, float volume, uint8_t kind);
void audio_voice_stop(VoiceHandle voice);
void audio_voice_mix_into(int32_t *acc, uint32_t frames);

//audio_sfx.c
const SfxDesc *audio_sfx_get(Sfx sfx);

#endif /* __AUDIO_INTERNAL_H */
