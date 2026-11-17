#include "audio_internal.h"

#include <stdint.h>
#include <string.h>

static int32_t master_volume = AUDIO_MASTER_MAX_GAIN;
void audio_set_master_volume(float volume) {
    if (volume > 1.0f) {
        volume = 1.0f;
    }
    if (volume < 0) {
        volume = 0;
    }
    master_volume = (int32_t)(AUDIO_MASTER_MAX_GAIN * volume);
}

VoiceHandle audio_play_sfx(Sfx sfx, float volume) {
    const SfxDesc *desc = audio_sfx_get(sfx);
    if (!desc) {
        return AUDIO_VOICE_INVALID;
    }
    VoiceHandle voice = audio_voice_alloc(AUDIO_MAX_MUSIC_VOICES, AUDIO_MAX_VOICES);
    if (voice == AUDIO_VOICE_INVALID) {
        return voice;
    }
    audio_voice_start(voice, desc->data, desc->len, desc->format, desc->sample_rate, volume, VOICE_SFX);
    return voice;
}

void audio_mix(int16_t *out) {
    static int32_t acc[AUDIO_HALF_FRAMES];
    memset(acc, 0, sizeof(acc));

    audio_voice_mix_into(acc, AUDIO_HALF_FRAMES);

    for (uint32_t i = 0; i < AUDIO_HALF_FRAMES; ++i) {
        int32_t scaled = (acc[i] * master_volume) >> 8;

        if (scaled > INT16_MAX) {
            scaled = INT16_MAX;
        }
        if (scaled < INT16_MIN) {
            scaled = INT16_MIN;
        }
        out[2 * i] = (int16_t)scaled;
        out[2 * i + 1] = (int16_t)scaled;
    }
}
