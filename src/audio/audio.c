#include "audio.h"

#include <stdint.h>
#include <assert.h>
#include <string.h>

#include "mem_sections.h"

// default max volume chosen via on device testing
#define AUDIO_MASTER_MAX 4000
static int16_t master_volume = AUDIO_MASTER_MAX;
void audio_set_master_volume(float volume) {
    assert(volume <= 1.0 && volume >= 0);
    master_volume = (int16_t)(AUDIO_MASTER_MAX * volume);
}

// Testing melody
static uint32_t frames_per_beat = 120; // suitable default
void audio_set_frames_per_beat_from_bpm(uint32_t bpm) {
    assert(bpm <= 240);
    assert(bpm > 0);
    frames_per_beat = AUDIO_SAMPLE_RATE * 60 / bpm;
}

typedef struct {
    float freq;
    uint8_t beats;
} Note;

static const Note song[] = {
    {261.6f, 1},
    {329.6f, 1},
    {392.0f, 1},
    {523.3f, 2},
    {0, 1},
};
// _testing melody

// SFX
#define AUDIO_SFX_POOL_SIZE 128000u
#define AUDIO_SFX_MAX_NUMBER 64u
#define AUDIO_MAX_VOICES 16u
#define AUDIO_SFX_INVALID 0xFFFFu
typedef struct {
    uint32_t offset;
    uint32_t bytes;
    uint16_t sample_rate;
    uint8_t format;
} SfxDesc;

AXI_BSS static uint8_t sfx_pool[AUDIO_SFX_POOL_SIZE];
static uint32_t sfx_used; // bmp pointer
static SfxDesc sfx_table[AUDIO_SFX_MAX_NUMBER];
static uint16_t sfx_count;
typedef uint16_t Sfx; // handle to index into sfx_table

// copies into sfx pool and returns handle.
Sfx audio_load_sfx(const void *pcm, uint32_t bytes, uint16_t sample_rate, uint8_t format) {

    if (bytes > AUDIO_SFX_POOL_SIZE - sfx_used) {
        return AUDIO_SFX_INVALID;
    }
    if (sfx_count == AUDIO_SFX_MAX_NUMBER) {
        return AUDIO_SFX_INVALID;
    }

    uint32_t will_use = sfx_used + ((bytes + 3u) & ~3u);
    if (will_use > AUDIO_SFX_POOL_SIZE) {
        return AUDIO_SFX_INVALID;
    }

    memcpy(sfx_pool + sfx_used, pcm, bytes);

    sfx_table[sfx_count++] = (SfxDesc){
        .offset = sfx_used,
        .bytes = bytes,
        .sample_rate = sample_rate,
        .format = format,
    };

    sfx_used = will_use;
    return sfx_count - 1;
}
// SFX

//TODO(vasilis): this is a tmp sequencer for a single voice.
//Needs to be an actual mixer :D
void audio_mix(int16_t *out) {
    static uint32_t note_idx = 0;
    static uint32_t frames_left = 0;
    static uint32_t step = 0;
    static uint32_t phase = 0;

    for (uint32_t i = 0; i < AUDIO_HALF_FRAMES; ++i) {
        if (frames_left == 0) {
            frames_left = song[note_idx].beats * frames_per_beat;
            step = (uint32_t)(song[note_idx].freq * 4294967296.0f / AUDIO_SAMPLE_RATE);
            note_idx = (note_idx + 1) % (sizeof(song) / sizeof(*song));
        }
        frames_left--;

        phase += step;
        int16_t level = (step == 0) ? 0 : ((phase & 0x80000000u) ? 1000 : -1000);
        out[2 * i] = level;
        out[2 * i + 1] = level;
    }
}
