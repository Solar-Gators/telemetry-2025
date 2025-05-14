/*
Nathan Achinger
Ublox MAX M10S Driver
*/
#pragma once

#include "main.h"

#include <cstring>
#include <cstdio>

#ifdef USING_FREERTOS
#include "FreeRTOS.h"
#include "cmsis_os2.h"
#include "semphr.h"
#endif

#define GPS_BUFFER_SIZE 1024

class MaxM10S {
public:
	struct GNSSFixData {
	    double latitude_deg;
	    double longitude_deg;
	    float speed_knots;
	    float course_deg;
	    uint8_t fix_valid;
	    uint8_t fix_mode;
	    uint8_t satellites_used;
	    uint16_t date;
	    uint32_t time;
	};

	MaxM10S() = default;
	void init(I2C_HandleTypeDef* hi2c);

	GNSSFixData getFixData();
	void readOutputBuffer();
	void parseNMEA();

private:
	uint16_t getDataLength();
	void parseGNRMC(char* sentence);
	void parseGNGGA(char* sentence);
	bool checksumValid(uint8_t* data, uint8_t length);

	GNSSFixData fix_data;

	uint8_t gps_buffer[GPS_BUFFER_SIZE];
	#ifdef USING_FREERTOS
	SemaphoreHandle_t buffer_mutex;
	#endif
	volatile uint16_t gps_head = 0;
	volatile uint16_t gps_tail = 0;

	I2C_HandleTypeDef* i2c_handle;

	static constexpr uint8_t I2C_ADDRESS = 0x42;
	static constexpr uint16_t LEN_REG_HIGH = 0xFD;
	static constexpr uint8_t DATA_REG = 0xFF;
};

extern MaxM10S maxm10s_inst;
