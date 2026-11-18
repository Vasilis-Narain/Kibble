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

// Adds the loaded `sfx` to the voices that need to get mixed and returns VoiceHandle.
// VoiceHandle is then used for all the generic audio functions (_stop, _pause, _resume...)
VoiceHandle audio_play_sfx(Sfx sfx) {
    const SfxDesc *desc = audio_sfx_get_desc(sfx);
    if (!desc) {
        return AUDIO_VOICE_INVALID;
    }

    VoiceHandle voice = audio_voice_sfx_alloc(sfx, desc);
    if (voice == AUDIO_VOICE_INVALID) {
        return voice;
    }
    audio_voice_start(voice, sfx, desc->data, desc->len, desc->format, desc->sample_rate, desc->volume, VOICE_SFX);
    return voice;
}

void audio_stop(VoiceHandle voice) {
    audio_voice_stop(voice);
}

void audio_pause(VoiceHandle voice) {
    audio_voice_pause(voice);
}

void audio_resume(VoiceHandle voice) {
    audio_voice_resume(voice);
}

bool audio_is_playing(VoiceHandle voice) {
    return audio_voice_isplaying(voice);
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
