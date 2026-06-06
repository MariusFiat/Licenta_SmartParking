#ifndef __ADC_TEST_H
#define __ADC_TEST_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Inject the adc_if header file*/
#include "../../../src/mcal/adc_if.h"
#include "../../../src/board_config.h"

uint32_t* mock_hw_cs_reg(uint32_t addr);

#ifdef __cplusplus
}
#endif

#define MOCK_ADC_CS_ADDR     0x40001000
#define MOCK_ADC_RESULT_ADDR 0x40001004

#undef x_ADC_CS_REG
#undef x_ADC_RESULT_REG
#undef xHW_REG32

#define x_ADC_CS_REG      MOCK_ADC_CS_ADDR
#define x_ADC_RESULT_REG  MOCK_ADC_RESULT_ADDR

#define xHW_REG32(addr) (*mock_hw_cs_reg(addr))

#endif