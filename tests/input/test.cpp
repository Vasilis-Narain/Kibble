#include "CppUTest/TestHarness.h"

extern "C" {
    #include "input.h"
}

// Create a test group
TEST_GROUP(inputs_test_group){
    // init before each test
    void setup(){
        input_adc_register_centres(2000,2000);
        input_update(0, 0, 0);
        input_update(0, 0, 0);
    }

    // deinit after each test (like dealloc)
    void teardown() {
    }
}; 

TEST(inputs_test_group, adc_scale) {
    input_update(0, 3000, 1000);
    DOUBLES_EQUAL(input_get_joy_axis_movement(AXIS_X), 0.5, 0.2);
    DOUBLES_EQUAL(input_get_joy_axis_movement(AXIS_Y), -0.5, 0.2);
}

TEST(inputs_test_group, adc_scale_min_max) {
    input_update(0, 4095, 0);
    DOUBLES_EQUAL(input_get_joy_axis_movement(AXIS_X), 1, 0);
    DOUBLES_EQUAL(input_get_joy_axis_movement(AXIS_Y), -1, 0);
    input_update(0, 0, 4095);
    DOUBLES_EQUAL(input_get_joy_axis_movement(AXIS_Y), 1, 0);
    DOUBLES_EQUAL(input_get_joy_axis_movement(AXIS_X), -1, 0);
}

TEST(inputs_test_group, press_is_pressed_and_down) {
    input_update(BTN_INPUT_MASK(INPUT_A), 0,  0);
    CHECK_TRUE(input_is_key_pressed(INPUT_A));
    CHECK_TRUE(input_is_key_down(INPUT_A));
    CHECK_FALSE(input_is_key_up(INPUT_A));
}

TEST(inputs_test_group, second_frame_is_held_not_pressed) {
    input_update(BTN_INPUT_MASK(INPUT_A), 0, 0);
    input_update(BTN_INPUT_MASK(INPUT_A), 0, 0);
    CHECK_FALSE(input_is_key_pressed(INPUT_A));
    CHECK_TRUE(input_is_key_held(INPUT_A));
}

TEST(inputs_test_group, release_is_released_and_up) {
    input_update(BTN_INPUT_MASK(INPUT_A), 0, 0);
    input_update(0, 0, 0);
    CHECK_TRUE(input_is_key_released(INPUT_A));
    CHECK_TRUE(input_is_key_up(INPUT_A));
    CHECK_FALSE(input_is_key_down(INPUT_A));
}

TEST(inputs_test_group, released_only_for_one_frame) {
    input_update(BTN_INPUT_MASK(INPUT_A), 0, 0);
    input_update(0, 0, 0);
    input_update(0, 0, 0);
    CHECK_FALSE(input_is_key_released(INPUT_A));
}

TEST(inputs_test_group, nothing_pressed) {
    CHECK_FALSE(input_is_key_pressed(INPUT_A));
    CHECK_FALSE(input_is_key_down(INPUT_A));
    CHECK_TRUE(input_is_key_up(INPUT_A));
}

TEST(inputs_test_group, btns_independent) {
    input_update(BTN_INPUT_MASK(INPUT_A), 0, 0);
    CHECK_FALSE(input_is_key_pressed(INPUT_B));
    CHECK_FALSE(input_is_key_down(INPUT_B));
    CHECK_TRUE(input_is_key_up(INPUT_B));
}

TEST(inputs_test_group, multiple_buttons) {
    input_update(BTN_INPUT_MASK(INPUT_A) | BTN_INPUT_MASK(INPUT_B) | BTN_INPUT_MASK(INPUT_DOWN), 0, 0);
    CHECK_TRUE(input_is_key_down(INPUT_B));
    CHECK_TRUE(input_is_key_down(INPUT_A));
    CHECK_TRUE(input_is_key_down(INPUT_DOWN));
}

TEST(inputs_test_group, press_again_after_release) {
    input_update(BTN_INPUT_MASK(INPUT_A), 0, 0);
    input_update(0, 0, 0);
    input_update(BTN_INPUT_MASK(INPUT_A), 0, 0);
    CHECK_TRUE(input_is_key_pressed(INPUT_A));
}
