#ifndef __AUDIO_H
#define __AUDIO_H

#include <stdint.h>

#define AUDIO_SAMPLE_RATE 48000u
#define AUDIO_HALF_FRAMES 256u
#define AUDIO_HALF_SAMPLES (AUDIO_HALF_FRAMES * 2u)

void audio_init(void);
void audio_mix(int16_t *out);
void audio_set_frames_per_beat_from_bpm(uint32_t bpm);

#endif /* __AUDIO_H */
