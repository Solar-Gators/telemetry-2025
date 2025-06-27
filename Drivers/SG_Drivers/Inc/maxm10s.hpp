/*
Nathan Achinger
Ublox MAX M10S Driver
*/
#pragma once

#include "main.h"

#include <cstring>
#include <cstdio>
#include <stdlib.h>
#include <string>

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
	    float ground_speed_knots;
	    float course_deg;
	    uint8_t fix_valid;
	    uint8_t fix_mode;
	    uint8_t num_satellites;
	    uint8_t quality;
	    uint16_t date;
	    std::string time;
	};

	MaxM10S(I2C_HandleTypeDef* hi2c);
	void init();;
	const GNSSFixData getFixDataCopy();
	void readOutputBuffer();
	void parseNMEA();

private:
	uint16_t getDataLength();
	void parseGNRMC(char* sentence);
	void parseGNGGA(char* sentence);
	double nmeaToDecimal(const char* nmeaCoord, const char direction);
	bool NMEAchecksumValid(const char* sentence);

	GNSSFixData fix_data;

	#ifdef USING_FREERTOS
	SemaphoreHandle_t buffer_mutex = nullptr;
    SemaphoreHandle_t fix_data_mutex = nullptr;
	#endif

    volatile uint8_t gps_buffer[GPS_BUFFER_SIZE];
	volatile uint16_t gps_head = 0;
	volatile uint16_t gps_tail = 0;

	I2C_HandleTypeDef* i2c_handle;

	static constexpr uint8_t I2C_ADDRESS = 0x42;
	static constexpr uint16_t LEN_REG_HIGH = 0xFD;
	static constexpr uint8_t DATA_REG = 0xFF;
};

extern MaxM10S maxm10s; // Global instance of MaxM10S
