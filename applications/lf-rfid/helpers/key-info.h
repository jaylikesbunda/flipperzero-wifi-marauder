#pragma once
#include <stdint.h>

static const uint8_t LFRFID_KEY_SIZE = 8;

enum class LfrfidKeyType : uint8_t {
    KeyEM4100,
    KeyH10301,
    KeyI40134,
};

const char* lfrfid_key_get_type_string(LfrfidKeyType type);
uint8_t lfrfid_key_get_type_data_count(LfrfidKeyType type);