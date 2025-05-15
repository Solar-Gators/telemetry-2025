#pragma once

const char TELEMETRY_DATA_JSON_FORMAT[] =
"{\"req\":\"note.add\",\"sync\":true,\"body\":{\"bms\":{\"last_rx_time\":%u,\"main_batt_v\":%u,\"main_batt_c\":%u,\"low_cell_v\":%u,\"high_cell_v\":%u,\"high_cell_temp\":%u},\"gps\":{\"last_rx_time\":%u,\"latitude\":%u,\"longitude\":%u,\"speed\":%u,\"satellites_used\":%u}}}";
