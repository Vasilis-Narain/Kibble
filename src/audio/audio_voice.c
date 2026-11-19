/*
 * SPDX-FileCopyrightText: 2026 Vasilis Narain
 *
 * SPDX-License-Identifier: MIT
 */

#include "audio_internal.h"

#include <stdint.h>
#include <stddef.h>
#include <assert.h>
#include <stdbool.h>

#include "mem_sections.h"

typedef struct {
    const void *data;
    uint32_t pos;
    uint32_t step;
    uint32_t len;
    int32_t volume;
    Sfx sfx;
    uint8_t format;
    uint8_t flags;
    uint8_t paused;
    uint8_t gen;
} Voice;

static Voice voices[AUDIO_MAX_VOICES];

static inline uint8_t audio_voice_idx(VoiceHandle h) {
    return h & 0xFFu;
}

static inline uint8_t audio_voice_gen(VoiceHandle h) {
    return h >> 8;
}

// returns Voice ptr after doing bounds checks + gen_counter checks
static Voice *audio_voice_get(VoiceHandle h) {
    uint8_t idx = audio_voice_idx(h);
    uint8_t gen = audio_voice_gen(h);

    if (idx >= AUDIO_MAX_VOICES) {
        return NULL;
    }
    if (voices[idx].gen != gen) {
        return NULL;
    }
    return &voices[idx];
}

// finds next free voice
static VoiceHandle audio_voice_alloc(uint32_t first, uint32_t last) {
    for (uint32_t i = first; i < last; ++i) {
        if (!(voices[i].flags & VOICE_ACTIVE)) {
            return (++voices[i].gen << 8) | i; // bump gen counter on allocation
        }
    }
    return AUDIO_VOICE_INVALID;
}

VoiceHandle audio_voice_sfx_alloc(Sfx sfx, const SfxDesc *desc) {
    uint32_t count = 0;
    uint32_t max_pos_index = AUDIO_VOICE_INVALID;
    uint32_t max_pos = 0;

    // count how many active instances of the current sfx we have in voices
    for (uint32_t i = AUDIO_MAX_MUSIC_VOICES; i < AUDIO_MAX_VOICES; ++i) {
        if ((voices[i].flags & VOICE_ACTIVE) && voices[i].sfx == sfx) {
            count++;

            if (voices[i].pos >= max_pos) {
                max_pos = voices[i].pos;
                max_pos_index = i;
            }
        }
    }

    // if we can have more instances we alloc as usual
    if (count < desc->max_instances) {
        return audio_voice_alloc(AUDIO_MAX_MUSIC_VOICES, AUDIO_MAX_VOICES);
    }

    // resets 'oldest' (as in the voice with the max pos) voice
    return (++voices[max_pos_index].gen << 8) | max_pos_index; // bump gen counter on allocation
}

void audio_voice_start(VoiceHandle voice, Sfx sfx, const void *data, uint32_t len, uint8_t format, uint16_t rate, int32_t volume, uint8_t kind) {
    uint8_t idx = audio_voice_idx(voice);
    assert(idx < AUDIO_MAX_VOICES);

    // ensure the mixer skips this voice while we're updating it
    voices[idx].flags = 0;

    COMPILER_FENCE;

    voices[idx].sfx = sfx;
    voices[idx].data = data;
    voices[idx].format = format;
    voices[idx].len = len;
    voices[idx].pos = 0;
    voices[idx].step = ((uint32_t)rate << 16) / AUDIO_SAMPLE_RATE;
    voices[idx].volume = volume;
    voices[idx].paused = 0;

    COMPILER_FENCE;

    voices[idx].flags = VOICE_ACTIVE | kind;
}

static void mix_pcm16(Voice *v, int32_t *acc, uint32_t frames) {
    const int16_t *data = v->data;
    for (uint32_t i = 0; i < frames; ++i) {
        acc[i] += (data[v->pos >> 16] * v->volume) >> 8;
        v->pos += v->step;
        if ((v->pos >> 16) >= v->len) {
            v->flags = 0;
            return;
        }
    }
}

static void mix_pcm8(Voice *v, int32_t *acc, uint32_t frames) {
    const uint8_t *data = v->data;
    for (uint32_t i = 0; i < frames; ++i) {
        acc[i] += ((((int32_t)data[v->pos >> 16] - 128) * 256) * v->volume) >> 8; // * 256 rather than << 8 cause left shift on neg input is UB
        v->pos += v->step;
        if ((v->pos >> 16) >= v->len) {
            v->flags = 0;
            return;
        }
    }
}
// adds every voice into acc
void audio_voice_mix_into(int32_t *acc, uint32_t frames) {
    for (uint32_t v_index = 0; v_index < AUDIO_MAX_VOICES; ++v_index) {
        Voice *v = &voices[v_index];
        if (!(v->flags & VOICE_ACTIVE) || v->paused) {
            continue;
        }
        switch (v->format) {
        case SFX_PCM16:
            mix_pcm16(v, acc, frames);
            break;

        case SFX_PCM8:
            mix_pcm8(v, acc, frames);
            break;

        default: {
            v->flags = 0;
            break;
        }
        }
    }
}

bool audio_voice_isplaying(VoiceHandle voice) {
    Voice *v = audio_voice_get(voice);
    if (!v) {
        return false;
    }
    return !v->paused && (v->flags & VOICE_ACTIVE);
}

// will pause if active and playing
// no effect if not active
void audio_voice_pause(VoiceHandle voice) {
    Voice *v = audio_voice_get(voice);
    if (!v) {
        return;
    }
    v->paused = 1;
}
void audio_voice_resume(VoiceHandle voice) {
    Voice *v = audio_voice_get(voice);
    if (!v) {
        return;
    }
    v->paused = 0;
}

void audio_voice_stop(VoiceHandle voice) {
    Voice *v = audio_voice_get(voice);
    if (!v) {
        return;
    }
    v->flags = 0;
}

void audio_voice_stop_many(uint32_t start, uint32_t end) {
    assert(start < AUDIO_MAX_VOICES);
    assert(end <= AUDIO_MAX_VOICES);
    for (; start < end; ++start) {
        voices[start].flags = 0;
    }
}
