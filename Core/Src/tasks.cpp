#include "tasks.hpp"

extern "C" {
#include "main.h"
}

extern "C" void CPP_UserSetup(void) {
	maxm10s.init();
	notecard.init();
	rfd900x.init();
}

extern "C" void StartTXData(void* argument) {
	while(1) {
		osMutexAcquire(transmitter_ptr_mutex, osWaitForever);
		if (transmitter == nullptr) {
			Error_Handler();
		}
		transmitter->loadGPSData(maxm10s.getFixDataCopy());
		transmitter->send();

		osMutexRelease(transmitter_ptr_mutex);
		osDelay(5000);
	}
}

extern "C" void StartGPSReadBuffer(void* argument) {
	while(1) {
		maxm10s.readOutputBuffer();

		HAL_GPIO_TogglePin(OK_LED_GPIO_Port, OK_LED_Pin);
		osDelay(500);
	}
}

extern "C" void StartGPSParseNMEA(void* argument) {
	while(1) {
		maxm10s.parseNMEA();
		osDelay(200);
	}
}

extern "C" void StartCANParseFrames(void *argument)
{
	while(1) {
		transmitter->updateFrame();
		osDelay(200);
	}
}
