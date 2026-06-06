#include <gtest/gtest.h>
#include "adc_test.h"

/* Variables that will simulate the hardware registers */
uint32_t mock_adc_cs = 0;
uint32_t mock_adc_result = 0;
uint32_t mock_dummy_reg = 0; 

/* Hardware register simulation, this simulates the hardware ready response. */
extern "C" uint32_t* mock_hw_cs_reg(uint32_t addr) {
    if (addr == MOCK_ADC_CS_ADDR) {

        if (mock_adc_cs & (1 << x_ADC_START_ONCE_BIT)) {
            mock_adc_cs |= (1 << x_ADC_READY_BIT);
            mock_adc_result = 0xABC;
        }
        return &mock_adc_cs;
    }
    else if (addr == MOCK_ADC_RESULT_ADDR) {
        return &mock_adc_result;
    }

    return &mock_dummy_reg; 
}

/* C header files */
extern "C" {
    #include "../../../src/mcal/adc_if.c"
}

class Adc_IF : public ::testing::Test {
protected:
    void SetUp() override {   
        /* Initialize with default values */
        mock_adc_cs = 0; 
        mock_adc_result = 0;
        mock_dummy_reg = 0;
    }
};

TEST_F(Adc_IF, Adc_IF_MASKS) {
    uint16_t result = x_read_ambient_light_once();
    
    /* Check if the hardware was initialized correctly and the request was received and the conversion was completed */
    uint32_t expected_start_bit = (1 << x_ADC_START_ONCE_BIT) | (1 << x_ADC_READY_BIT); 
    EXPECT_EQ(mock_adc_cs, expected_start_bit);
}