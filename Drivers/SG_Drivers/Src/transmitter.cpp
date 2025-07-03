#include "transmitter.hpp"
#include "FreeRTOS.h"
#include "semphr.h"
#include "secret.h"
#include "notecard_format.h"
#include <cstdio>
#include <cstring>

CanFrame can_frames[MAX_NUM_FRAMES];
QueueHandle_t canRxQueue = nullptr;

Notecard notecard(&huart2);
RFD900X rfd900x(&huart1);

Transmitter* transmitter = &notecard;
SemaphoreHandle_t transmitter_ptr_mutex = xSemaphoreCreateMutex();

Transmitter::Transmitter(UART_HandleTypeDef *huart) {
	huart_ = huart;
}

QueueHandle_t Transmitter::getCanRxQueue() {
	if (canRxQueue == nullptr) {
		Error_Handler();
	}
	return canRxQueue;
}

void Transmitter::updateFrame() {
	CanFrame frame;
	if (xQueueReceive(getCanRxQueue(), &frame, portMAX_DELAY) == pdPASS) {
		// Parse frame data for Notecard
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

			case 0x05:
				telem_data->low_cell_v = ((uint16_t)frame.data[1] << 8) | (uint16_t)frame.data[0];
				telem_data->high_cell_v = ((uint16_t)frame.data[3] << 8) | (uint16_t)frame.data[2];
				telem_data->high_cell_t = ((uint16_t)frame.data[5] << 8) | (uint16_t)frame.data[4];
				telem_data->cell_idx_low_v = frame.data[6];
				telem_data->cell_idx_high_t = frame.data[7];
				break;

			case MPPT1_CAN_BASE_ID:
				telem_data->mppt1_input_v = ((uint32_t)frame.data[7] << 24) |
											((uint32_t)frame.data[6] << 16) |
											((uint32_t)frame.data[5] << 8)  |
											((uint32_t)frame.data[4]);

				telem_data->mppt1_input_c = ((uint32_t)frame.data[3] << 24) |
											((uint32_t)frame.data[2] << 16) |
											((uint32_t)frame.data[1] << 8)  |
											((uint32_t)frame.data[0]);
				break;

			case MPPT1_CAN_BASE_ID + 1:
				telem_data->mppt1_output_v = ((uint32_t)frame.data[7] << 24) |
											 ((uint32_t)frame.data[6] << 16) |
											 ((uint32_t)frame.data[5] << 8)  |
											 ((uint32_t)frame.data[4]);

				telem_data->mppt1_output_c = ((uint32_t)frame.data[3] << 24) |
											 ((uint32_t)frame.data[2] << 16) |
											 ((uint32_t)frame.data[1] << 8)  |
											 ((uint32_t)frame.data[0]);
				break;

			case MPPT2_CAN_BASE_ID:
				telem_data->mppt2_input_v = ((uint32_t)frame.data[7] << 24) |
											((uint32_t)frame.data[6] << 16) |
											((uint32_t)frame.data[5] << 8)  |
											((uint32_t)frame.data[4]);

				telem_data->mppt2_input_c = ((uint32_t)frame.data[3] << 24) |
											((uint32_t)frame.data[2] << 16) |
											((uint32_t)frame.data[1] << 8)  |
											((uint32_t)frame.data[0]);
				break;

			case MPPT2_CAN_BASE_ID + 1:
				telem_data->mppt2_output_v = ((uint32_t)frame.data[7] << 24) |
											((uint32_t)frame.data[6] << 16) |
											((uint32_t)frame.data[5] << 8)  |
											((uint32_t)frame.data[4]);

				telem_data->mppt2_output_c = ((uint32_t)frame.data[3] << 24) |
											((uint32_t)frame.data[2] << 16) |
											((uint32_t)frame.data[1] << 8)  |
											((uint32_t)frame.data[0]);
				break;

			case MPPT3_CAN_BASE_ID:
				telem_data->mppt3_input_v = ((uint32_t)frame.data[7] << 24) |
											((uint32_t)frame.data[6] << 16) |
											((uint32_t)frame.data[5] << 8)  |
											((uint32_t)frame.data[4]);

				telem_data->mppt3_input_c = ((uint32_t)frame.data[3] << 24) |
											((uint32_t)frame.data[2] << 16) |
											((uint32_t)frame.data[1] << 8)  |
											((uint32_t)frame.data[0]);
				break;

			case MPPT3_CAN_BASE_ID + 1:
				telem_data->mppt3_output_v = ((uint32_t)frame.data[7] << 24) |
											 ((uint32_t)frame.data[6] << 16) |
											 ((uint32_t)frame.data[5] << 8)  |
											 ((uint32_t)frame.data[4]);

				telem_data->mppt3_output_c = ((uint32_t)frame.data[3] << 24) |
											 ((uint32_t)frame.data[2] << 16) |
											 ((uint32_t)frame.data[1] << 8)  |
											 ((uint32_t)frame.data[0]);
				break;

			case MITSUBA_CAN_ID_FRAME0:
				// Bits [9:0]
				telem_data->motor_ctrl_v = ((uint16_t)frame.data[1] & 0b00000011) << 8 | frame.data[0];

				// Bits [29:20]
				telem_data->motor_ctrl_c = (((uint16_t)(frame.data[2] >> 4)) & 0x0F) << 6 |
                        				   ((uint16_t)(frame.data[3]) & 0x3F);
				break;

			case MITSUBA_CAN_ID_FRAME1:
				// FUTURE FRAME
				break;

			case MITSUBA_CAN_ID_FRAME2:
				// CONTAINS ERROR FLAGS
				break;
		}

		// Update frame data for RFD900X
		for (int i = 0; i < num_frames; i++) {
			if (can_data[i].can_id == frame.can_id) {
				memcpy(can_data[i].data, frame.data, 8);
				return;
			}
		}

		// Add new CAN frame
		if (num_frames == MAX_NUM_FRAMES) {
			Error_Handler();
		}
		can_data[num_frames].can_id = frame.can_id;
		memcpy(can_data[num_frames].data, frame.data, 8);
		num_frames++;
	}
}

RFD900X::RFD900X(UART_HandleTypeDef *huart) : Transmitter(huart) {}

void RFD900X::init() {
	if (canRxQueue == nullptr) {
		canRxQueue = xQueueCreate(32, sizeof(CanFrame));
	}
}

void RFD900X::send() {
	; // TO DO: Add RFD900X send implementation
}

Notecard::Notecard(UART_HandleTypeDef *huart) : Transmitter(huart) {}

void Notecard::init() {
  if (canRxQueue == nullptr) {
	canRxQueue = xQueueCreate(16, sizeof(CanFrame));
  }
  char setup[] = "{\"req\":\"hub.set\", \"product\":\"com.gmail.solargators.swe:flare_telem\", \"mode\":\"continuous\"}\n";
  //char setup[] = "{\"req\":\"hub.set\", \"product\":\"edu.ufl.nathan.achinger:flare_telemetry\", \"mode\":\"continuous\"}\n";
  HAL_UART_Transmit(huart_, (uint8_t*)setup, sizeof(setup), HAL_MAX_DELAY);
}

void Notecard::send() {
	int32_t longitude_scaled = (int32_t)(fix_data.longitude_deg * 1e6);
	int32_t latitude_scaled = (int32_t)(fix_data.latitude_deg * 1e6);
	uint32_t ground_speed = (uint32_t)(fix_data.ground_speed_knots * 100);

	char tx_buffer[1024];;
	uint16_t tx_size = snprintf(
			tx_buffer,
			sizeof(tx_buffer),
			TELEMETRY_DATA_JSON_FORMAT,
			telem_data->sup_bat_v, telem_data->main_bat_v,
			telem_data->main_bat_c, telem_data->low_cell_v,
			telem_data->high_cell_v, telem_data->high_cell_t,
			telem_data->cell_idx_low_v, telem_data->cell_idx_high_t,
			telem_data->mppt1_input_v, telem_data->mppt1_input_c,
			telem_data->mppt1_output_v, telem_data->mppt1_output_c,
			telem_data->mppt2_input_v, telem_data->mppt2_input_c,
			telem_data->mppt2_output_v, telem_data->mppt2_output_c,
			telem_data->mppt3_input_v, telem_data->mppt3_input_c,
			telem_data->mppt3_output_v, telem_data->mppt3_output_c,
			telem_data->motor_ctrl_v, telem_data->motor_ctrl_c,
			(long)longitude_scaled, (long)latitude_scaled,
			(long)(ground_speed), fix_data.num_satellites
	);
	HAL_UART_Transmit(huart_, (uint8_t*)tx_buffer, tx_size, HAL_MAX_DELAY);
}
