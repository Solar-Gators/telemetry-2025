#pragma once
#include "stm32l4xx_hal.h"
#include "transmitter.hpp"

#ifdef __cplusplus
extern "C" {
#endif

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan);

#ifdef __cplusplus
}
#endif
