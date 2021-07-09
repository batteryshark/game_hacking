#pragma once
#include <cstdint>

#define MAX_8BIT   255
#define MAX_10BIT 1024

#define __bswap_constant_16(x) ((uint16_t) ((((x) >> 8) & 0xFF) | (((x) & 0xFF) << 8)))


namespace GunIOUtils {
    uint16_t CONVERT_16bit_to_10bitLE(uint16_t in_value);

    uint8_t CONVERT_16bit_to_8bit(uint16_t in_value);

    uint8_t CalculateChecksum(const uint8_t *in_data, uint32_t in_length);
}