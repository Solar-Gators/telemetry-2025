#pragma once

#include "main.h"
#include "maxm10s.hpp"
#include "FreeRTOS.h"
#include "semphr.h"

#define MAX_NUM_FRAMES 15

typedef struct {
    uint32_t can_id;
    uint8_t data[8];
    uint8_t len;
} CanFrame;

typedef struct {
	uint32_t sup_bat_v = 0;
	uint32_t main_bat_v = 0;
	uint32_t main_bat_c = 0;
	uint16_t low_cell_v = 0;
	uint16_t high_cell_v = 0;
	uint16_t low_cell_t = 0;
	uint16_t high_cell_t= 0;
	uint8_t cell_idx_low_v = 0;
	uint8_t cell_idx_high_t = 0;

	// (uint16_t)(value * 100);  // 1 unit = 10 mV = 0.01 V
	// float decoded_voltage = transmitted_voltage * 0.01f;

	// decoded_value = transmitted_value * 0.01f;
	uint32_t mppt1_input_v = 0;
	uint32_t mppt1_input_c = 0;
	uint32_t mppt1_output_v= 0;
	uint32_t mppt1_output_c = 0;

	uint32_t mppt2_input_v = 0;
	uint32_t mppt2_input_c = 0;
	uint32_t mppt2_output_v= 0;
	uint32_t mppt2_output_c = 0;

	uint32_t mppt3_input_v = 0;
	uint32_t mppt3_input_c = 0;
	uint32_t mppt3_output_v = 0;
	uint32_t mppt3_output_c = 0;

	uint16_t motor_ctrl_v = 0; // voltage = voltage_raw * 0.5f
	uint16_t motor_ctrl_c = 0; // current = current_raw * 1.0f
	uint16_t motor_rpm = 0; // rpm = rpm_raw * 1.0f

} TelemData;

class Transmitter {
public:
	Transmitter(UART_HandleTypeDef *huart);
	virtual ~Transmitter() {
		delete[] can_data;
		delete telem_data;
	}

	static QueueHandle_t getCanRxQueue();

	virtual void init() = 0;
	void updateFrame();
	void loadGPSData(const MaxM10S::GNSSFixData &data) {
		fix_data = data;
	}
	virtual void send() = 0;

protected:
	UART_HandleTypeDef *huart_;
	CanFrame* can_data = new CanFrame[10];
	TelemData* telem_data = new TelemData;
	MaxM10S::GNSSFixData fix_data;

	uint8_t num_frames = 0;
};

class Notecard : public Transmitter {
public:
	Notecard(UART_HandleTypeDef *huart);
	void init() override;
	void send() override;
};

class RFD900X : public Transmitter {
public:
	RFD900X(UART_HandleTypeDef *huart);
	void init() override;
	void send() override;
};

extern CanFrame can_frames[MAX_NUM_FRAMES];

extern Transmitter* transmitter;
extern SemaphoreHandle_t transmitter_ptr_mutex;

extern Notecard notecard;
extern RFD900X rfd900x;
