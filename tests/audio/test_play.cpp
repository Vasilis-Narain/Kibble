#include "CppUTest/TestHarness.h"

#include "test_data.h"
extern "C" {
    #include "audio_internal.h"
}

// Create a test group
TEST_GROUP(audio_play_test_group){
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
