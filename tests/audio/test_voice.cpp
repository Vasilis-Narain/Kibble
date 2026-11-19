// SPDX-FileCopyrightText: 2026 Vasilis Narain
//
// SPDX-License-Identifier: MIT

#include "CppUTest/TestHarness.h"

#include "test_data.h"
extern "C" {
    #include "audio_internal.h"
    #include "string.h"
}

// Create a test group
TEST_GROUP(audio_voice_test_group){
    // init before each test
    void setup(){
        audio_sfx_reset();
        audio_voice_stop_many(0, AUDIO_MAX_VOICES);
        audio_set_master_volume(1.0f);
    }

    // deinit after each test (like dealloc)
    void teardown() {
    }
}; 

TEST(audio_voice_test_group, alloc_and_start_good_input) {
    Sfx sound = audio_sfx_load(clip16, CLIP16_LEN, 1.0f, 48000, SFX_PCM16, 1);
    CHECK_TRUE(sound != AUDIO_SFX_INVALID);
    VoiceHandle voice = audio_play_sfx(sound);
    CHECK_EQUAL(AUDIO_MAX_MUSIC_VOICES, voice);
}

TEST(audio_voice_test_group, alloc_and_start_past_max_instance) {
    Sfx sound = audio_sfx_load(clip16, CLIP16_LEN, 1.0f, 48000, SFX_PCM16, 1);
    CHECK_TRUE(sound != AUDIO_SFX_INVALID);
    VoiceHandle voice = audio_play_sfx(sound);
    CHECK_EQUAL(AUDIO_MAX_MUSIC_VOICES, voice);
    voice = audio_play_sfx(sound);
    CHECK_EQUAL(AUDIO_MAX_MUSIC_VOICES, voice);
}

TEST(audio_voice_test_group, alloc_and_start_mult_instance) {
    Sfx sound = audio_sfx_load(clip16, CLIP16_LEN, 1.0f, 48000, SFX_PCM16, 4);
    CHECK_TRUE(sound != AUDIO_SFX_INVALID);
    VoiceHandle voice;
    for (size_t i = 0; i < 4; ++i) {
        voice = audio_play_sfx(sound);
        CHECK_EQUAL(i + AUDIO_MAX_MUSIC_VOICES, voice);
    }
}

TEST(audio_voice_test_group, pcm16_voice_mix_into) {
    Sfx sound = audio_sfx_load(clip16, CLIP16_LEN, 1.0f, 48000, SFX_PCM16, 2);
    CHECK_TRUE(sound != AUDIO_SFX_INVALID);

    // Play the same voice twice (max instance = 2)
    VoiceHandle voice1 = audio_play_sfx(sound);
    CHECK_EQUAL(AUDIO_MAX_MUSIC_VOICES, voice1);
    VoiceHandle voice2 = audio_play_sfx(sound);
    CHECK_EQUAL(1 + AUDIO_MAX_MUSIC_VOICES, voice2);

    int32_t acc[AUDIO_HALF_FRAMES] = {0};
    audio_voice_mix_into(acc, AUDIO_HALF_FRAMES);

    for (size_t i = 0; i < CLIP16_LEN; ++i) {
        CHECK_EQUAL(clip16[i] * 2, acc[i]);
    }
    for (size_t i = CLIP16_LEN; i < AUDIO_HALF_FRAMES; ++i) {
        CHECK_EQUAL(0, acc[i]);
    }
}

TEST(audio_voice_test_group, pcm16_voice_mix_into_pause_1) {
    Sfx sound = audio_sfx_load(clip16, CLIP16_LEN, 1.0f, 48000, SFX_PCM16, 2);
    CHECK_TRUE(sound != AUDIO_SFX_INVALID);

    // Play the same voice twice (max instance = 2)
    VoiceHandle voice1 = audio_play_sfx(sound);
    CHECK_EQUAL(AUDIO_MAX_MUSIC_VOICES, voice1);
    VoiceHandle voice2 = audio_play_sfx(sound);
    CHECK_EQUAL(1 + AUDIO_MAX_MUSIC_VOICES, voice2);

    // pause voice
    audio_voice_pause(voice2);

    int32_t acc[AUDIO_HALF_FRAMES] = {0};
    audio_voice_mix_into(acc, AUDIO_HALF_FRAMES);

    for (size_t i = 0; i < CLIP16_LEN; ++i) {
        CHECK_EQUAL(clip16[i], acc[i]);
    }
    for (size_t i = CLIP16_LEN; i < AUDIO_HALF_FRAMES; ++i) {
        CHECK_EQUAL(0, acc[i]);
    }
}

TEST(audio_voice_test_group, pcm16_voice_mix_into_pause_1_resume) {
    Sfx sound = audio_sfx_load(clip16, CLIP16_LEN, 1.0f, 48000, SFX_PCM16, 2);
    CHECK_TRUE(sound != AUDIO_SFX_INVALID);

    // Play the same voice twice (max instance = 2)
    VoiceHandle voice1 = audio_play_sfx(sound);
    CHECK_EQUAL(AUDIO_MAX_MUSIC_VOICES, voice1);
    VoiceHandle voice2 = audio_play_sfx(sound);
    CHECK_EQUAL(1 + AUDIO_MAX_MUSIC_VOICES, voice2);

    // pause voice
    audio_voice_pause(voice2);
    
    int32_t acc[AUDIO_HALF_FRAMES] = {0};
    audio_voice_mix_into(acc, AUDIO_HALF_FRAMES);

    audio_voice_resume(voice2);

    // now voice1 should have finished
    memset(acc, 0, AUDIO_HALF_FRAMES);
    audio_voice_mix_into(acc, AUDIO_HALF_FRAMES);

    for (size_t i = 0; i < CLIP16_LEN; ++i) {
        CHECK_EQUAL(clip16[i], acc[i]);
    }
    for (size_t i = CLIP16_LEN; i < AUDIO_HALF_FRAMES; ++i) {
        CHECK_EQUAL(0, acc[i]);
    }
}

TEST(audio_voice_test_group, pcm16_voice_mix_into_then_stop_all) {
    Sfx sound = audio_sfx_load(clip16, CLIP16_LEN, 1.0f, 48000, SFX_PCM16, 2);
    CHECK_TRUE(sound != AUDIO_SFX_INVALID);

    // Play the same voice twice (max instance = 2)
    VoiceHandle voice1 = audio_play_sfx(sound);
    CHECK_EQUAL(AUDIO_MAX_MUSIC_VOICES, voice1);
    VoiceHandle voice2 = audio_play_sfx(sound);
    CHECK_EQUAL(1 + AUDIO_MAX_MUSIC_VOICES, voice2);
    
    int32_t acc[AUDIO_HALF_FRAMES] = {0};
    audio_voice_mix_into(acc, AUDIO_HALF_FRAMES);
    for (size_t i = 0; i < CLIP16_LEN; ++i) {
        CHECK_EQUAL(clip16[i] * 2, acc[i]);
    }

    audio_voice_stop_many(0, AUDIO_MAX_VOICES);
    memset(acc, 0, AUDIO_HALF_FRAMES);
    audio_voice_mix_into(acc, AUDIO_HALF_FRAMES);

    for (size_t i = 0; i < AUDIO_HALF_FRAMES; ++i) {
        CHECK_EQUAL(0, acc[i]);
    }
}

