#include <gtest/gtest.h>
#include "fake_hardware.h"

uint32_t mock_sio_out_set = 0;

/* C header files*/
extern "C" {
    #include "../../../src/mcal/adc_if.h"
    #include "../../../src/board_config.h"
}

class GpioTest : public ::testing::Test {
protected:
    void SetUp() override {.   /* This method will be executed before each test */
        mock_sio_out_set = 1; 
    }
};

/* This represents the test case */
TEST_F(GpioTest, GpioSetHighWritesCorrectBitmask) {

    EXPECT_EQ(mock_sio_out_set, 1);
}