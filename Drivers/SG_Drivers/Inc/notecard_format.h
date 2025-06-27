#pragma once
/*
const uint8_t TELEMETRY_DATA_JSON_FORMAT[] =
"{\"req\": \"note.add\", \"live\":true, \"sync\":true,\"body\":{\"bms\":{\"rx_time\":28465,\"main_batt_v\":110,\"main_batt_c\":12,\"low_cell_v\":14,\"high_cell_v\":5,\"high_cell_tmp\":8},\"gps\":{\"rx_time\":68576,\"latitude\":695,\"longitude\":285,\"speed\":24,\"num_sats\":10}}}\n";
*/

const char TELEMETRY_DATA_JSON_FORMAT[] =
"{\"req\": \"note.add\", \"live\":true, \"sync\":true,\"body\":{\"gps\":{\"rx_time\":0,\"longitude\":%ld,\"latitude\":%ld,\"speed\":17,\"num_sats\":%u}}}\n";
