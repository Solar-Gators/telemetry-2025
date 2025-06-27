#include "maxm10s.hpp"

MaxM10S maxm10s(&hi2c2);

MaxM10S::MaxM10S(I2C_HandleTypeDef* hi2c) {
	i2c_handle = hi2c;
}

void MaxM10S::init() {
	#ifdef USING_FREERTOS
	buffer_mutex = xSemaphoreCreateMutex();
	fix_data_mutex = xSemaphoreCreateMutex();

	if (buffer_mutex == nullptr) {
	    Error_Handler();
	}
	if (fix_data_mutex == nullptr) {
	    Error_Handler();
	}
	#endif

	/*
	uint8_t disableNMEA_enableUBX[] = { 0xB5, 0x62, 0x06, 0x8A, 0x14, 0x00, 0x00, 0x01, 0x00, 0x00, 0x01, 0x00, 0x72, 0x10, 0x01, 0x00, 0x00, 0x00, 0x02, 0x00, 0x72, 0x10, 0x00, 0x00, 0x00, 0x00, 0xD5, 0x10 };
	HAL_I2C_Master_Transmit(i2c_handle, (I2C_ADDRESS << 1), disableNMEA_enableUBX, 28, HAL_MAX_DELAY);
	*/
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
				#ifdef USING_FREERTOS
				osMutexRelease(buffer_mutex);
				#endif
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

	if (!NMEAchecksumValid(sentence)) {
		printf("Invalid checksum: %s\n", sentence);
		return; // Invalid checksum, discard sentence
	}

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

// Parses a GNRMC sentence to extract time, latitude, and longitude
void MaxM10S::parseGNRMC(char* sentence) {
	char* saveptr;
	char* token = strtok_r(sentence, ",", &saveptr); // Tokenize the string by commas

    int index = 0;                        // Keeps track of field

    while (token != nullptr) {
        switch (index) {
            case 1:
                // Field 1: UTC time in hhmmss.sss format
            	fix_data.time = token;
                break;

            case 3: {
                // Field 3: Latitude (in NMEA format)
                const char* latStr = token;
                const char * latDir = strtok_r(nullptr, ",", &saveptr); // Field 4: N/S
                index++;

                fix_data.latitude_deg = nmeaToDecimal(latStr, *latDir);
                break;
            }

            case 5: {
                // Field 5: Longitude (in NMEA format)
                const char* lonStr = token;
                const char* lonDir = strtok_r(nullptr, ",", &saveptr); // Field 6: E/W
                index++;

                fix_data.longitude_deg = nmeaToDecimal(lonStr, *lonDir);
                break;
            }

            case 7:
				// Field 7: Speed over ground in knots
				fix_data.ground_speed_knots = atof(token);
				break;
        }

        // Move to next comma-separated token
        token = strtok_r(nullptr, ",", &saveptr);
        index++;
    }
}

// Parses a GNGGA sentence to extract time, position, fix status, satellites, HDOP, and altitude
void MaxM10S::parseGNGGA(char* sentence) {
	char* saveptr;

    char* token = strtok_r(sentence, ",", &saveptr);  // Tokenize by commas
    int index = 0;                        // Field index

    while (token != nullptr) {
        switch (index) {
            case 1:
                // Field 1: UTC time
            	fix_data.time = token;
                break;

            case 2: {
                // Field 2: Latitude
                const char* latStr = token;
                const char* latDir = strtok_r(nullptr, ",", &saveptr);  // Field 3: N/S
                index++;

                fix_data.latitude_deg = nmeaToDecimal(latStr, *latDir);
                break;
            }

            case 4: {
                // Field 4: Longitude
                const char* lonStr = token;
                const char* lonDir = strtok_r(nullptr, ",", &saveptr);  // Field 5: E/W
                index++;

                fix_data.longitude_deg = nmeaToDecimal(lonStr, *lonDir);
                break;
            }

            case 6:
                // Field 6: Fix quality (0 = invalid, 1 = GPS, 2 = DGPS)
                fix_data.quality = atoi(token);
                break;

            case 7:
                // Field 7: Number of satellites in use
                fix_data.num_satellites = atoi(token);
                break;
        }

        // Next token
        token = strtok_r(nullptr, ",", &saveptr);
        index++;
    }
}

double MaxM10S::nmeaToDecimal(const char* nmeaCoord, const char direction) {
	double raw = atof(nmeaCoord);  // Convert string to double

	// Separate degrees and minutes
	int degrees = (int)(raw / 100);
	double minutes = raw - (degrees * 100);

	// Convert to decimal degrees
	double decimal = degrees + (minutes / 60.0);

	// Apply sign based on direction
	if (direction == 'S' || direction == 'W') {
		decimal = -decimal;
	}

	return decimal;
}

const MaxM10S::GNSSFixData MaxM10S::getFixDataCopy() {
	GNSSFixData copy;
	#ifdef USING_FREERTOS
	xSemaphoreTake(fix_data_mutex, portMAX_DELAY);
	#endif

	copy = fix_data;

	#ifdef USING_FREERTOS
	xSemaphoreGive(fix_data_mutex);
	#endif

	return copy;
}

bool MaxM10S::NMEAchecksumValid(const char* sentence) {
	if (sentence[0] != '$') return false;

	    uint8_t checksum = 0;
	    const char* p = sentence + 1;

	    // XOR all chars until '*'
	    while (*p && *p != '*') {
	        checksum ^= (uint8_t)(*p);
	        p++;
	    }

	    if (*p != '*') return false; // '*' not found

	    // Parse the checksum after '*'
	    uint8_t received_checksum = (uint8_t)strtol(p + 1, NULL, 16);

	    return (checksum == received_checksum);
}
