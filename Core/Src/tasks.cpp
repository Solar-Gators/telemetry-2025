#include "tasks.hpp"

extern "C" void StartTXData(void* argument) {
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
		maxm10s_inst.readOutputBuffer();
		HAL_GPIO_TogglePin(OK_LED_GPIO_Port, OK_LED_Pin);
		HAL_Delay(500);
	}
}

extern "C" void StartGPSParseNMEA(void* argument) {
	while(1) {
		maxm10s_inst.parseNMEA();
		HAL_Delay(200);
	}
}
