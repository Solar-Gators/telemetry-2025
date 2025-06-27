#pragma once

#include "stm32l4xx_hal.h"
#include "cmsis_os.h"

#include "Transmitter.hpp"
#include "maxm10s.hpp"

#ifdef __cplusplus
extern "C" {
#endif

// These are the FreeRTOS task functions CubeMX calls
void CPP_UserSetup(void);
void StartTXData(void* argument);
void StartGPSReadBuffer(void* argument);
void StartGPSParseNMEA(void* argument);
void StartCANParseFrames(void *argument);

#ifdef __cplusplus
}
#endif
