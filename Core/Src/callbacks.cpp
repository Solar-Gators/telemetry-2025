#include "callbacks.hpp"

extern QueueHandle_t canRxQueue;
/*
extern "C" void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan) {
	HAL_GPIO_WritePin(STATUS_LED_GPIO_Port, STATUS_LED_Pin, GPIO_PIN_SET);
    CAN_RxHeaderTypeDef rxHeader;
    uint8_t rxData[8];

    if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &rxHeader, rxData) == HAL_OK) {
        if (transmitter == nullptr) { Error_Handler(); }

        CanFrame rx_frame;
        rx_frame.message_id = rxData[0];
        memcpy(rx_frame.data, rxData + 1, 7);

        transmitter->updateFrame(rx_frame);
    }
    else { Error_Handler(); }
}
*/
extern "C" void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan) {
	CanFrame rx_frame;

	CAN_RxHeaderTypeDef header;
	uint8_t frame_data[8];
	if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &header, frame_data) == HAL_OK) {
		if (header.IDE == CAN_ID_STD) {
			rx_frame.can_id = header.StdId;
		}
		else if (header.IDE == CAN_ID_EXT) {
			rx_frame.can_id = header.ExtId;
		}
		memcpy(rx_frame.data, frame_data, 8);

		BaseType_t xHigherPriorityTaskWoken = pdFALSE;
		xQueueSendFromISR(Transmitter::getCanRxQueue(), &rx_frame, &xHigherPriorityTaskWoken);
		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
	}

	//HAL_GPIO_TogglePin(STATUS_LED_GPIO_Port, STATUS_LED_Pin);
}


extern "C" void HAL_CAN_RxFifo1MsgPendingCallback(CAN_HandleTypeDef *hcan) {
	CanFrame rx_frame;

	CAN_RxHeaderTypeDef header;
	uint8_t frame_data[8];
	if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &header, frame_data) == HAL_OK) {
		if (header.IDE == CAN_ID_STD) {
			rx_frame.can_id = header.StdId;
		}
		else if (header.IDE == CAN_ID_EXT) {
			rx_frame.can_id = header.ExtId;
		}
		memcpy(rx_frame.data, frame_data, 8);

		BaseType_t xHigherPriorityTaskWoken = pdFALSE;
		xQueueSendFromISR(Transmitter::getCanRxQueue(), &rx_frame, &xHigherPriorityTaskWoken);
		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
	}

	//HAL_GPIO_TogglePin(STATUS_LED_GPIO_Port, STATUS_LED_Pin);
}
