#include "transmitter.hpp"
#include "FreeRTOS.h"
#include "semphr.h"

Notecard notecard;
RFD900X rfd900x;

Transmitter* transmitter = &notecard;
SemaphoreHandle_t transmitter_ptr_mutex = xSemaphoreCreateMutex();

void Transmitter::loadFrame(const CAN_RxHeaderTypeDef& header, const uint8_t* data) {
	// TO DO: Add frame loading
}

void Notecard::init(UART_HandleTypeDef *huart) {
	huart_ = huart;
}

void Notecard::send() {
	// TO DO: Add Notecard send implementation
}

void RFD900X::init(UART_HandleTypeDef *huart) {
	huart_ = huart;
}

void RFD900X::send() {
	// TO DO: Add RFD900X send implementation
}
