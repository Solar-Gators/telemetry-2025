#include "transmitter.hpp"
#include "FreeRTOS.h"
#include "semphr.h"
#include "secret.h"
#include "notecard_format.h"
#include <cstdio>
#include <cstring>

CanFrame can_frames[MAX_NUM_FRAMES];

Notecard notecard(&huart2);
RFD900X rfd900x(&huart1);

Transmitter* transmitter = &notecard;
SemaphoreHandle_t transmitter_ptr_mutex = xSemaphoreCreateMutex();

Transmitter::Transmitter(UART_HandleTypeDef *huart) {
	huart_ = huart;
}

QueueHandle_t Transmitter::getCanRxQueue() {
	static QueueHandle_t canRxQueue = nullptr;
	if (canRxQueue == nullptr) {
		canRxQueue = xQueueCreate(32, sizeof(CanFrame));
	}
	return canRxQueue;
}

void Transmitter::updateFrame() {
	CanFrame frame;
	if (xQueueReceive(getCanRxQueue(), &frame, portMAX_DELAY) == pdPASS) {
		// Update frame data
		for (int i = 0; i < num_frames; i++) {
			if (can_data[i].can_id == frame.can_id) {
				memcpy(can_data[i].data, frame.data, 8);
				return;
			}
		}

		switch (frame.can_id) {
		case 0x03:
			telem_data->sup_bat_v = ((uint32_t)frame.data[4] << 24) |
									((uint32_t)frame.data[3] << 16) |
									((uint32_t)frame.data[2] << 8)  |
									((uint32_t)frame.data[1]);
			break;

		case 0x04:
			telem_data->main_bat_v = ((uint32_t)frame.data[3] << 24) |
									 ((uint32_t)frame.data[2] << 16) |
									 ((uint32_t)frame.data[1] << 8)  |
									 ((uint32_t)frame.data[0]);

			telem_data->main_bat_c = ((uint32_t)frame.data[7] << 24) |
									 ((uint32_t)frame.data[6] << 16) |
									 ((uint32_t)frame.data[5] << 8)  |
									 ((uint32_t)frame.data[4]);
			break;

		case 0x05:
			telem_data->low_cell_v = ((uint16_t)frame.data[1] << 8) | (uint16_t)frame.data[0];
			telem_data->high_cell_v = ((uint16_t)frame.data[3] << 8) | (uint16_t)frame.data[2];
			telem_data->high_cell_t = ((uint16_t)frame.data[5] << 8) | (uint16_t)frame.data[4];
			telem_data->cell_idx_low_v = frame.data[6];
			telem_data->cell_idx_high_t = frame.data[7];
			break;

		case 0x06:

			break;
		}

		if (num_frames == MAX_NUM_FRAMES) {
			Error_Handler();
		}
		// Add new CAN frame
		can_data[num_frames].can_id = frame.can_id;
		memcpy(can_data[num_frames].data, frame.data, 8);
		num_frames++;
	}
}

RFD900X::RFD900X(UART_HandleTypeDef *huart) : Transmitter(huart) {}

void RFD900X::init() {
	; // TO DO: Add RFD900X initialization
}

void RFD900X::send() {
	; // TO DO: Add RFD900X send implementation
}

Notecard::Notecard(UART_HandleTypeDef *huart) : Transmitter(huart) {}

void Notecard::init() {
  uint8_t setup[] = "{\"req\":\"hub.set\", \"product\": \"edu.ufl.nathan.achinger:flare_telemetry\", \"mode\": \"continuous\"}\n";
  HAL_UART_Transmit(huart_, setup, sizeof(setup)/sizeof(setup[0]), HAL_MAX_DELAY);
}

void Notecard::send() {
	char tx_buffer[256];

	int32_t longitude_scaled = (int32_t)(fix_data.longitude_deg * 1e6);
	int32_t latitude_scaled = (int32_t)(fix_data.latitude_deg * 1e6);
	uint16_t tx_size = snprintf(tx_buffer, sizeof(tx_buffer), TELEMETRY_DATA_JSON_FORMAT, (long)longitude_scaled, (long)latitude_scaled, fix_data.num_satellites);
	HAL_UART_Transmit(huart_, (uint8_t*)tx_buffer, tx_size, HAL_MAX_DELAY);
}
