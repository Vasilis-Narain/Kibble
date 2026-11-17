#include "audio_internal.h"

#include <stdint.h>
#include <string.h>

#include "mem_sections.h"

AXI_BSS static uint8_t sfx_pool[AUDIO_SFX_POOL_SIZE];
static uint32_t sfx_pool_used; // bmp pointer
static SfxDesc sfx_table[AUDIO_SFX_MAX_NUMBER];
static uint16_t sfx_count;

static uint32_t sfx_bytes(uint8_t format, uint32_t len) {
    switch (format) {
    case SFX_PCM8:
        return len;
    case SFX_PCM16:
        return len * 2u;
    case SFX_ADPCM:
        return (len + 1u) / 2u; // 4 bits per sample, round up
    default:
        return 0; // unknown format → load fails
    }
}

// copies into sfx pool and returns handle.
Sfx audio_sfx_load(const void *pcm, uint32_t len, uint16_t sample_rate, uint8_t format) {
    if (len == 0 || len > AUDIO_SFX_POOL_SIZE) {
        return AUDIO_SFX_INVALID;
    }

    uint32_t bytes = sfx_bytes(format, len);
    uint32_t padded = (bytes + 3u) & ~3u;

    if (sfx_count == AUDIO_SFX_MAX_NUMBER) {
        return AUDIO_SFX_INVALID;
    }
    if (padded > AUDIO_SFX_POOL_SIZE - sfx_pool_used) {
        return AUDIO_SFX_INVALID;
    }

    memcpy(sfx_pool + sfx_pool_used, pcm, bytes);

    sfx_table[sfx_count] = (SfxDesc){
        .data = &sfx_pool[sfx_pool_used],
        .len = len,
        .sample_rate = sample_rate,
        .format = format,
    };

    sfx_pool_used += padded;
    return sfx_count++;
}

const SfxDesc *audio_sfx_get(Sfx sfx);
void audio_sfx_reset(void);
