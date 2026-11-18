#include "audio_internal.h"

#include <stdint.h>
#include <string.h>

#include "mem_sections.h"

AXI_BSS static uint8_t sfx_pool[AUDIO_SFX_POOL_SIZE];
static uint32_t sfx_pool_used; // bump pointer
static SfxDesc sfx_table[AUDIO_SFX_MAX_NUMBER];
static uint16_t sfx_count;

static uint32_t sfx_bytes(uint8_t format, uint32_t len) {
    switch (format) {
    case SFX_PCM8:
        return len;
    case SFX_PCM16:
        return len * 2u;
    case SFX_ADPCM:
        return (len + 1u) / 2u;
    default:
        return 0; // unknown format → load fails
    }
}

void audio_sfx_set_volume(Sfx sfx, float volume) {
    if (sfx >= sfx_count) {
        return;
    }
    if (volume >= 1.0f) {
        volume = 1.0f;
    }
    if (volume < 0.0f) {
        volume = 0.0f;
    }
    sfx_table[sfx].volume = (int32_t)(volume * 256.0f);
}

// copies into sfx pool and returns handle.
Sfx audio_sfx_load(const void *pcm, uint32_t len, float volume, uint16_t sample_rate, uint8_t format) {
    if (len == 0 || len > AUDIO_SFX_MAX_SAMPLE_SIZE || sample_rate == 0) {
        return AUDIO_SFX_INVALID;
    }

    uint32_t bytes = sfx_bytes(format, len);
    if (bytes == 0) {
        return AUDIO_SFX_INVALID;
    }

    uint32_t padded = (bytes + 3u) & ~3u;

    if (sfx_count == AUDIO_SFX_MAX_NUMBER) {
        return AUDIO_SFX_INVALID;
    }
    if (padded > AUDIO_SFX_POOL_SIZE - sfx_pool_used) {
        return AUDIO_SFX_INVALID;
    }

    memcpy(sfx_pool + sfx_pool_used, pcm, bytes);

    if (volume >= 1.0f) {
        volume = 1.0f;
    }
    if (volume < 0.0f) {
        volume = 0.0f;
    }

    sfx_table[sfx_count] = (SfxDesc){
        .data = &sfx_pool[sfx_pool_used],
        .len = len,
        .volume = (int32_t)(volume * 256.0f),
        .sample_rate = sample_rate,
        .format = format,
    };

    sfx_pool_used += padded;
    return sfx_count++;
}

const SfxDesc *audio_sfx_get(Sfx sfx) {
    return (sfx < sfx_count) ? &sfx_table[sfx] : NULL;
}

void audio_sfx_reset(void) {
    audio_voice_stop_many(AUDIO_MAX_MUSIC_VOICES, AUDIO_MAX_VOICES);
    sfx_pool_used = 0;
    sfx_count = 0;
}
