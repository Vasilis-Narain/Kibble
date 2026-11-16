#include "audio.h"
#include <stdint.h>
#include <assert.h>

// Testing melody
static uint32_t frames_per_beat;
void audio_set_frames_per_beat_from_bpm(uint32_t bpm) {
    assert(bpm <= 240);
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
