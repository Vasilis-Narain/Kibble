// SPDX-FileCopyrightText: 2026 Vasilis Narain
//
// SPDX-License-Identifier: MIT

#include "CppUTest/TestHarness.h"

#include "test_data.h"
extern "C" {
    #include "audio_internal.h"
}

// Create a test group
TEST_GROUP(audio_mix_test_group){
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

TEST(audio_mix_test_group, valid_mix_sequence) {
    int16_t out[AUDIO_HALF_SAMPLES];
    static const int16_t expected[] = {12, 4039, -4059, 2254};


    Sfx sfx_16 = audio_sfx_load(clip16, CLIP16_LEN, 1.0f, 48000, SFX_PCM16, 1);
    CHECK_TRUE(sfx_16 != AUDIO_SFX_INVALID);
    Sfx sfx_8 = audio_sfx_load(clip8, CLIP8_LEN, 1.0f, 48000, SFX_PCM8, 1);
    CHECK_TRUE(sfx_8 != AUDIO_SFX_INVALID);

    VoiceHandle voice_16 = audio_play_sfx(sfx_16);
    CHECK_TRUE(voice_16 != AUDIO_VOICE_INVALID);
    VoiceHandle voice_8 = audio_play_sfx(sfx_8);
    CHECK_TRUE(voice_8 != AUDIO_VOICE_INVALID);

    audio_mix(out);
    for (size_t i = 0; i < AUDIO_HALF_FRAMES; ++i) {
        int16_t e = (i < 4) ? expected[i] : 0;
        CHECK_EQUAL(e, out[2 * i]);
        CHECK_EQUAL(e, out[2 * i + 1]);
    }
}
