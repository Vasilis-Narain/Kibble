#include "audio_internal.h"

#include <stdint.h>
#include <assert.h>

#include "mem_sections.h"

typedef struct {
    const void *data;
    uint32_t pos;
    uint32_t step;
    uint32_t len;
    int32_t volume;
    uint8_t format;
    uint8_t flags;
    uint8_t paused;
} Voice;

static Voice voices[AUDIO_MAX_VOICES];

// finds next free voice
VoiceHandle audio_voice_alloc(uint32_t first, uint32_t last) {
    for (uint32_t i = first; i < last; ++i) {
        if (!(voices[i].flags & VOICE_ACTIVE)) {
            return i;
        }
    }
    return AUDIO_VOICE_INVALID;
}

void audio_voice_start(VoiceHandle voice, const void *data, uint32_t len, uint8_t format, uint16_t rate, int32_t volume, uint8_t kind) {
    voices[voice].data = data;
    voices[voice].format = format;
    voices[voice].len = len;
    voices[voice].pos = 0;
    voices[voice].step = ((uint32_t)rate << 16) / AUDIO_SAMPLE_RATE;
    voices[voice].volume = volume;
    voices[voice].paused = 0;

    COMPILER_FENCE;

    voices[voice].flags = VOICE_ACTIVE | kind;
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
        acc[i] += ((((int32_t)data[v->pos >> 16] - 128) << 8) * v->volume) >> 8;
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

void audio_voice_pause(VoiceHandle voice) {
    if (voice >= AUDIO_MAX_VOICES) {
        return;
    }
    voices[voice].paused = 1;
}
void audio_voice_resume(VoiceHandle voice) {
    if (voice >= AUDIO_MAX_VOICES) {
        return;
    }
    voices[voice].paused = 0;
}

void audio_voice_stop(VoiceHandle voice) {
    if (voice >= AUDIO_MAX_VOICES) {
        return;
    }
    voices[voice].flags = 0;
}

void audio_voice_stop_many(uint32_t start, uint32_t end) {
    assert(start < AUDIO_MAX_VOICES);
    assert(end <= AUDIO_MAX_VOICES);
    for (; start < end; ++start) {
        audio_voice_stop(start);
    }
}
