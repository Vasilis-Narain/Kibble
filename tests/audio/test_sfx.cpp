// SPDX-FileCopyrightText: 2026 Vasilis Narain
//
// SPDX-License-Identifier: MIT

#include "CppUTest/TestHarness.h"

#include "test_data.h"
extern "C" {
    #include "audio_internal.h"
}

// Create a test group
TEST_GROUP(audio_sfx_test_group){
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

// PCM16
TEST(audio_sfx_test_group, pcm16_load_valid_copies_data_and_fills_desc) {
    Sfx sound = audio_sfx_load(clip16, CLIP16_LEN, 1.0f, 48000u, SFX_PCM16, 1);
    CHECK_EQUAL(0, sound);
    const SfxDesc *desc = audio_sfx_get_desc(sound);
    CHECK(desc != NULL);
    CHECK(desc->data != clip16);
    MEMCMP_EQUAL(clip16, desc->data, sizeof(clip16));
    CHECK_EQUAL(CLIP16_LEN, desc->len);
    CHECK_EQUAL(48000, desc->sample_rate);
    CHECK_EQUAL(SFX_PCM16, desc->format);
    CHECK_EQUAL(256, desc->volume);
}

TEST(audio_sfx_test_group, pcm16_load_len_input_zero) {
    Sfx sound = audio_sfx_load(clip16, 0, 1.0f, 48000u, SFX_PCM16, 1);
    CHECK_EQUAL(AUDIO_SFX_INVALID, sound);
}

TEST(audio_sfx_test_group, pcm16_load_len_input_too_big) {
    Sfx sound = audio_sfx_load(clip16, AUDIO_SFX_MAX_SAMPLE_SIZE + 1, 1.0f, 48000u, SFX_PCM16, 1);
    CHECK_EQUAL(AUDIO_SFX_INVALID, sound);
}

TEST(audio_sfx_test_group, pcm16_load_too_many) {
    Sfx sound;
    for (size_t i = 0; i < AUDIO_SFX_MAX_NUMBER; ++i) {
        sound = audio_sfx_load(clip16, CLIP16_LEN, 1.0f, 48000u, SFX_PCM16, 1);
        CHECK_EQUAL(i, sound);
    }
    sound = audio_sfx_load(clip16, CLIP16_LEN, 1.0f, 48000u, SFX_PCM16, 1);
    CHECK_EQUAL(AUDIO_SFX_INVALID, sound);
}

TEST(audio_sfx_test_group, pcm16_load_full_reset_load_again) {
    Sfx sound;
    for (size_t i = 0; i < AUDIO_SFX_MAX_NUMBER; ++i) {
        sound = audio_sfx_load(clip16, CLIP16_LEN, 1.0f, 48000u, SFX_PCM16, 1);
        CHECK_EQUAL(i, sound);
    }
    audio_sfx_reset();
    for (size_t i = 0; i < AUDIO_SFX_MAX_NUMBER; ++i) {
        sound = audio_sfx_load(clip16, CLIP16_LEN, 1.0f, 48000u, SFX_PCM16, 1);
        CHECK_EQUAL(i, sound);
    }
}

// PCM8
TEST(audio_sfx_test_group, pcm8_load_valid_copies_data_and_fills_desc) {
    Sfx sound = audio_sfx_load(clip8, CLIP8_LEN, 1.0f, 48000u, SFX_PCM8, 1);
    CHECK_EQUAL(0, sound);
    const SfxDesc *desc = audio_sfx_get_desc(sound);
    CHECK(desc != NULL);
    CHECK(desc->data != clip8);
    MEMCMP_EQUAL(clip8, desc->data, sizeof(clip8));
    CHECK_EQUAL(CLIP8_LEN, desc->len);
    CHECK_EQUAL(48000, desc->sample_rate);
    CHECK_EQUAL(SFX_PCM8, desc->format);
    CHECK_EQUAL(256, desc->volume);
}

// BIGCLIP (size = max pool size)
TEST(audio_sfx_test_group, bigclip_load_valid) {
    Sfx sound = audio_sfx_load(big_clip, BIG_CLIP_LEN, 1.0f, 48000u, SFX_PCM16, 1);
    CHECK_EQUAL(0, sound);
    const SfxDesc *desc = audio_sfx_get_desc(sound);
    CHECK(desc != NULL);
    CHECK(desc->data != big_clip);
    MEMCMP_EQUAL(big_clip, desc->data, sizeof(big_clip));
    CHECK_EQUAL(BIG_CLIP_LEN, desc->len);
    CHECK_EQUAL(48000, desc->sample_rate);
    CHECK_EQUAL(SFX_PCM16, desc->format);
    CHECK_EQUAL(256, desc->volume);
}

TEST(audio_sfx_test_group, bigclip_load_then_try_load_more) {
    Sfx sound = audio_sfx_load(big_clip, BIG_CLIP_LEN, 1.0f, 48000u, SFX_PCM16, 1);
    CHECK_EQUAL(0, sound);
    sound = audio_sfx_load(clip16, CLIP16_LEN, 1.0f, 48000, SFX_PCM16, 1);
    CHECK_EQUAL(AUDIO_SFX_INVALID, sound);
}

TEST(audio_sfx_test_group, bigclip_load_len_input_max) {
    Sfx sound = audio_sfx_load(big_clip,  AUDIO_SFX_MAX_SAMPLE_SIZE, 1.0f, 48000u, SFX_PCM8, 1);
    CHECK_EQUAL(0, sound);
}
