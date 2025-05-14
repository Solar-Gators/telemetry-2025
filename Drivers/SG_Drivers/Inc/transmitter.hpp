#pragma once

#include "main.h"
#include "FreeRTOS.h"
#include "semphr.h"

class Transmitter {
public:
	Transmitter() = default;
	virtual ~Transmitter() {}

	void loadFrame(const CAN_RxHeaderTypeDef& header, const uint8_t* data);
	virtual void init(UART_HandleTypeDef *huart) = 0;
	virtual void send() = 0;
protected:
	uint8_t frame_data;
private:
	static Transmitter* instance;
};

class Notecard : public Transmitter {
public:
	void init(UART_HandleTypeDef *huart);
	void send() override;
private:
	UART_HandleTypeDef *huart_;
};

class RFD900X : public Transmitter {
public:
	void init(UART_HandleTypeDef *huart);
	void send() override;
private:
	UART_HandleTypeDef *huart_;
};

extern Transmitter* transmitter;
extern SemaphoreHandle_t transmitter_ptr_mutex;
