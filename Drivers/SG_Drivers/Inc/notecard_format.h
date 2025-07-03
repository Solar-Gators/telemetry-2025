#pragma once
/*
const char TELEMETRY_DATA_JSON_FORMAT[] =
    "{\"req\": \"note.add\","
    "\"live\":true,"
    "\"sync\":true,"
    "\"body\":{"
        "\"battery\":{"
            "\"sup_bat_v\":%u,"
            "\"main_bat_v\":%u,"
            "\"main_bat_c\":%u,"
            "\"low_cell_v\":%u,"
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
*/
/*
const char TELEMETRY_DATA_JSON_FORMAT[] =
"{\"req\":\"note.add\",\"live\":true,\"sync\":true,\"body\":{\"battery\":{\"sup_bat_v\":10,\"main_bat_v\":10,\"main_bat_c\":10,\"low_cell_v\":10,\"high_cell_v\":10,\"high_cell_t\":10,\"cell_idx_low_v\":10,\"cell_idx_high_t\":10},\"mppt1\":{\"input_v\":10,\"input_c\":10,\"output_v\":10,\"output_c\":10},\"mppt2\":{\"input_v\":10,\"input_c\":10,\"output_v\":10,\"output_c\":10},\"mppt3\":{\"input_v\":10,\"input_c\":10,\"output_v\":10,\"output_c\":10},\"mitsuba\":{\"voltage\":10,\"current\":10},\"gps\":{\"rx_time\":10,\"longitude\":10,\"latitude\":10,\"speed\":10,\"num_sats\":10}}}\n";
*/

const char TELEMETRY_DATA_JSON_FORMAT[] =
"{\"req\":\"note.add\",\"live\":true,\"sync\":true,\"body\":{\"battery\":{\"sup_bat_v\":%u,\"main_bat_v\":%u,\"main_bat_c\":%u,\"low_cell_v\":%u,\"high_cell_v\":%u,\"high_cell_t\":%u,\"cell_idx_low_v\":%u,\"cell_idx_high_t\":%u},\"mppt1\":{\"input_v\":%u,\"input_c\":%u,\"output_v\":%u,\"output_c\":%u},\"mppt2\":{\"input_v\":%u,\"input_c\":%u,\"output_v\":%u,\"output_c\":%u},\"mppt3\":{\"input_v\":%u,\"input_c\":%u,\"output_v\":%u,\"output_c\":%u},\"mitsuba\":{\"voltage\":%u,\"current\":%u},\"gps\":{\"rx_time\":-1,\"longitude\":%ld,\"latitude\":%ld,\"speed\":%ld,\"num_sats\":%u}}}\n";
