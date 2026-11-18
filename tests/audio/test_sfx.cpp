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

TEST(audio_sfx_test_group, load_valid_pcm16_copies_data_and_fills_desc) {
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

TEST(audio_sfx_test_group, load_len_input_zero) {
    Sfx sound = audio_sfx_load(clip16, 0, 1.0f, 48000u, SFX_PCM16, 1);
    CHECK_EQUAL(AUDIO_SFX_INVALID, sound);
}

TEST(audio_sfx_test_group, load_len_input_too_big) {
    Sfx sound = audio_sfx_load(clip16, UINT32_MAX, 1.0f, 48000u, SFX_PCM16, 1);
    CHECK_EQUAL(AUDIO_SFX_INVALID, sound);
}
