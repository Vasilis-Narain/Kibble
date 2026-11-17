#include "audio_internal.h"

#include <stdint.h>

typedef struct {
    const void *data;
    uint32_t pos;
    uint32_t step;
    uint32_t len;
    float volume;
    uint8_t format;
    uint8_t flags;
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

void audio_voice_start(VoiceHandle voice, const void *data, uint32_t len, uint8_t format, uint16_t rate, float volume, uint8_t kind);
void audio_voice_stop(VoiceHandle voice);

// adds every voice into acc
void audio_voice_mix_into(int32_t *acc, uint32_t frames);
