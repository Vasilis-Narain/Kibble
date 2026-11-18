#ifndef __AUDIO_H
#define __AUDIO_H

#include <stdint.h>
#include <stdbool.h>

#define AUDIO_SFX_INVALID 0xFFFFu
#define AUDIO_VOICE_INVALID 0xFFu

typedef uint16_t Sfx;
typedef uint8_t VoiceHandle;

void audio_init(void);
Sfx audio_sfx_load(const void *pcm, uint32_t len, float volume, uint16_t sample_rate, uint8_t format, uint8_t max_instances);
void audio_sfx_reset(void);

VoiceHandle audio_play_sfx(Sfx sfx);

void audio_stop(VoiceHandle voice);
void audio_pause(VoiceHandle voice);
void audio_resume(VoiceHandle voice);
bool audio_is_playing(VoiceHandle voice);

void audio_set_master_volume(float volume);

enum {
    SFX_PCM8 = 0,
    SFX_PCM16,
    SFX_ADPCM,
};

#endif // __AUDIO_H
