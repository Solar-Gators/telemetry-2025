#include "tasks.hpp"

extern "C" {
#include "main.h"
}

extern "C" void CPP_UserSetup() {
	rfd900x.init(&huart1);
	notecard.init(&huart2);
	maxm10s.init(&hi2c2);
}

extern "C" void StartTXData(void* argument) {
	osDelay(6000);
	while(1) {
		osMutexAcquire(transmitter_ptr_mutex, osWaitForever);
		if (transmitter != nullptr) {
			transmitter->send();
		}
		osMutexRelease(transmitter_ptr_mutex);
		HAL_Delay(5000);
	}
}

extern "C" void StartGPSReadBuffer(void* argument) {
	while(1) {
		maxm10s.readOutputBuffer();
		HAL_GPIO_TogglePin(OK_LED_GPIO_Port, OK_LED_Pin);
		HAL_Delay(500);
	}
}

extern "C" void StartGPSParseNMEA(void* argument) {
	while(1) {
		maxm10s.parseNMEA();
		HAL_Delay(200);
	}
}
