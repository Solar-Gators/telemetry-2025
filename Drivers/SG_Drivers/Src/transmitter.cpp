#include "transmitter.hpp"
#include "FreeRTOS.h"
#include "semphr.h"
#include "secret.h"
#include "notecard_format.h"
#include <cstdio>

Notecard notecard;
RFD900X rfd900x;

Transmitter* transmitter = &notecard;
SemaphoreHandle_t transmitter_ptr_mutex = xSemaphoreCreateMutex();

void Transmitter::loadFrame(const CAN_RxHeaderTypeDef& header, const uint8_t* data) {
	// TO DO: Add frame loading
}

void Notecard::init(UART_HandleTypeDef *huart) {
	huart_ = huart;
	HAL_UART_Transmit(huart_, (uint8_t*)NOTECARD_PRODUCT_UID, sizeof(NOTECARD_PRODUCT_UID) - 1, HAL_MAX_DELAY);
}

void Notecard::send() {
	uint8_t packet[] = "{\"req\":\"note.add\",\"body\":{\"param1\":200,\"param2\":400}}\n";
	HAL_UART_Transmit(huart_, packet, sizeof(packet)/sizeof(packet[0]), HAL_MAX_DELAY);

	/*
	static char packet[2048];
	uint16_t bytes_written = snprintf(packet, sizeof(packet), TELEMETRY_DATA_JSON_FORMAT,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
	);

	if (bytes_written >= sizeof(packet)) {
		Error_Handler();
	}

	HAL_UART_Transmit(huart_, (uint8_t*)packet, bytes_written, HAL_MAX_DELAY);

	char sync[] = "{\"req\":\"hub.sync\"}\n";
	HAL_UART_Transmit(huart_, (uint8_t*)sync, sizeof(sync), HAL_MAX_DELAY);
	*/
}

void RFD900X::init(UART_HandleTypeDef *huart) {
	huart_ = huart;
}

void RFD900X::send() {
	// TO DO: Add RFD900X send implementation
}
