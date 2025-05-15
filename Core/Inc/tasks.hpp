#pragma once

#include "stm32l4xx_hal.h"
#include "cmsis_os.h"

#include "Transmitter.hpp"
#include "maxm10s.hpp"

#ifdef __cplusplus
extern "C" {
#endif

void CPP_UserSetup();

// These are the FreeRTOS task functions CubeMX calls
void StartTXData(void* argument);
void StartGPSReadBuffer(void* argument);
void StartGPSParseNMEA(void* argument);

#ifdef __cplusplus
}
#endif
