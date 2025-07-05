#pragma once
/*
const char TELEMETRY_DATA_JSON_FORMAT_TEST[] =
    "{\"req\": \"note.add\","
    "\"live\":true,"
    "\"sync\":true,"
    "\"body\":{"
        "\"battery\":{"
            "\"sup_bat_v\":%u," // this is in mV so divide by 1,000
            "\"main_bat_v\":%u," // this is in mV so divide by 1,000
            "\"main_bat_c\":%u," //
            "\"low_cell_v\":%u," // this is
            "\"high_cell_v\":%u,"
            "\"high_cell_t\":%u,"
            "\"cell_idx_low_v\":%u,"
            "\"cell_idx_high_t\":%u"
        "},"
        "\"mppt1\":{"
            "\"input_v\":%u,"
            "\"input_c\":%u,"
            "\"output_v\":%u,"
            "\"output_c\":%u"
        "},"
        "\"mppt2\":{"
            "\"input_v\":%u,"
            "\"input_c\":%u,"
            "\"output_v\":%u,"
            "\"output_c\":%u"
        "},"
        "\"mppt3\":{"
            "\"input_v\":%u,"
            "\"input_c\":%u,"
            "\"output_v\":%u,"
            "\"output_c\":%u"
        "},"
        "\"mitsuba\":{"
            "\"voltage\":%u,"
            "\"current\":%u"
        "},"
        "\"gps\":{"
            "\"rx_time\":\"10\","
            "\"longitude\":%ld,"
            "\"latitude\":%ld,"
            "\"speed\":%u,"
            "\"num_sats\":%u"
        "}"
    "}"
    "}\n";
const char TELEMETRY_DATA_JSON_FORMAT[] = R"({
	    "req": "note.add",
	    "live": true,
	    "sync": true,
	    "body": {
	        "battery": {
	            "high_cell_t": %u,
	            "high_cell_v": %u,
	            "low_cell_v": %u,
	            "main_bat_c": %.3f,
	            "main_bat_v": %u,
	            "sup_bat_v": %u
	        },
	        "mppt1": {
	            "input_v": %.3f,
	            "input_c": %.3f,
	            "output_v": %.3f,
	            "output_c": %.3f
	        },
	        "mppt2": {
	            "input_v": %.3f,
	            "input_c": %.3f,
	            "output_v": %.3f,
	            "output_c": %.3f
	        },
	        "mppt3": {
	            "input_v": %.3f,
	            "input_c": %.3f,
	            "output_v": %.3f,
	            "output_c": %.3f
	        }
	    }
	}
	)";
*/

const char TELEMETRY_DATA_JSON_FORMAT[] =
"{\"req\":\"note.add\",\"live\":true,\"sync\":true,\"body\":{\"battery\":{\"high_cell_t\":%u,\"high_cell_v\":%u,\"low_cell_v\":%u,\"main_bat_c\":%.3f,\"main_bat_v\":%u,\"sup_bat_v\":%u},\"mppt1\":{\"input_c\":%.3f,\"input_v\":%.3f,\"output_c\":%.3f,\"output_v\":%.3f},\"mppt2\":{\"input_c\":%.3f,\"input_v\":%.3f,\"output_c\":%.3f,\"output_v\":%.3f},\"mppt3\":{\"input_c\":%.3f,\"input_v\":%.3f,\"output_c\":%.3f,\"output_v\":%.3f}}}\n";
