#include "maxm10s.hpp"

MaxM10S maxm10s;

void MaxM10S::init(I2C_HandleTypeDef* hi2c) {
	i2c_handle = hi2c;

	#ifdef USING_FREERTOS
	buffer_mutex = xSemaphoreCreateMutex();
	#endif

	uint8_t disableNMEA_enableUBX[] = { 0xB5, 0x62, 0x06, 0x8A, 0x14, 0x00, 0x00, 0x01, 0x00, 0x00, 0x01, 0x00, 0x72, 0x10, 0x01, 0x00, 0x00, 0x00, 0x02, 0x00, 0x72, 0x10, 0x00, 0x00, 0x00, 0x00, 0xD5, 0x10 };
	HAL_I2C_Master_Transmit(i2c_handle, (I2C_ADDRESS << 1), disableNMEA_enableUBX, 28, HAL_MAX_DELAY);
}

void MaxM10S::readOutputBuffer() {
	uint16_t bytes_available = getDataLength();
	uint8_t rx_buff[256];

	if (bytes_available > 0) {
		if (bytes_available > 256) {
			bytes_available = 256;
		}

		HAL_I2C_Mem_Read(i2c_handle, (I2C_ADDRESS << 1), DATA_REG, I2C_MEMADD_SIZE_8BIT, rx_buff, bytes_available, HAL_MAX_DELAY);

		#ifdef USING_FREERTOS
		osMutexAcquire(buffer_mutex, osWaitForever);
		#endif

		for (uint16_t i = 0; i < bytes_available; i++) {
			uint16_t next_head = (gps_head + 1) % GPS_BUFFER_SIZE;

			if (next_head == gps_tail) {
				return; // buffer is full discard new data
			}

			gps_buffer[gps_head] = rx_buff[i];
			gps_head = next_head;
		}
		#ifdef USING_FREERTOS
		osMutexRelease(buffer_mutex);
		#endif
	}
}

void MaxM10S::parseNMEA() {
	char sentence[128];
	uint16_t start;
	uint16_t end;

	#ifdef USING_FREERTOS
	osMutexAcquire(buffer_mutex, osWaitForever);
	#endif

	bool valid_sentence_found = false;

	uint16_t i = gps_tail;

	while (i != gps_head)
	{
		if (gps_buffer[i] == '$')
			break; // Found start of a sentence
		i = (i + 1) % GPS_BUFFER_SIZE; // Move to next byte
	}

	if (i == gps_head) {
		#ifdef USING_FREERTOS
		osMutexRelease(buffer_mutex);
		#endif
		return;
	}

	start = i;

	while (i != gps_head) {
		if (gps_buffer[i] == '\n') {
			valid_sentence_found = true;
			break;
		}
		i = (i + 1) % GPS_BUFFER_SIZE;
	}

	if (!valid_sentence_found) {
		#ifdef USING_FREERTOS
		osMutexRelease(buffer_mutex);
		#endif
		return;
	}

	end = i;

	uint16_t j = start;
	uint16_t idx = 0;
	while (j != (end + 1) % GPS_BUFFER_SIZE) {
		sentence[idx++] = gps_buffer[j];
		j = (j + 1) % GPS_BUFFER_SIZE;
	}
	sentence[idx] = '\0';
	gps_tail = (end + 1) % GPS_BUFFER_SIZE; // update tail

	#ifdef USING_FREERTOS
	osMutexRelease(buffer_mutex);
	#endif

	if (strncmp(sentence, "$GNRMC", 6) == 0)
	{
		parseGNRMC(sentence); // Call parsing function for GNRMC
	}
	else if (strncmp(sentence, "$GNGGA", 6) == 0)
	{
		parseGNGGA(sentence); // Call parsing function for GNGGA
	}

	return;
}

uint16_t MaxM10S::getDataLength() {
	uint8_t error_count = 0;
	HAL_StatusTypeDef status;

	uint8_t rx[2];
	status = HAL_I2C_Mem_Read(i2c_handle, (I2C_ADDRESS << 1), LEN_REG_HIGH, I2C_MEMADD_SIZE_8BIT, rx, 2, HAL_MAX_DELAY);
	error_count += (status != HAL_OK);

	if (error_count > 0) {
		return -1;
	}

	return ((uint16_t)rx[0] << 8) | rx[1];
}

void MaxM10S::parseGNRMC(char* sentence) {
	printf("GNRMC received: %s\n", sentence);
}

void MaxM10S::parseGNGGA(char* sentence) {
	printf("GNGGA received: %s\n", sentence);
}

MaxM10S::GNSSFixData MaxM10S::getFixData() {
	return fix_data;
}

bool MaxM10S::checksumValid(uint8_t* response, uint8_t length) {
	uint8_t CK_A, CK_B;
	for (int i = 0; i < length - 2; i++) {
		CK_A += response[i];
		CK_B += CK_A;
	}
	// Check that the checksum matches the last two bytes of the response
	if (CK_A == response[length - 2] && CK_B == response[length - 1]) {
		return true;
	}
	return false;
}
