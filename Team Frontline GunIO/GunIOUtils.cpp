#include "GunIOUtils.h"


uint8_t CalculateChecksum(const uint8_t *in_data, uint32_t in_length) {
    unsigned char cks = 0;
    unsigned int i = 0;
    for (i = 0; i < in_length; i++) {
        cks = cks - in_data[i] & 0xFF;
    }
    return cks;
}


// This may have to be expanded.
// 16 * ~8bit_value & 0xFF0;
uint8_t CONVERT_16bit_to_8bit(uint16_t in_value){
    uint8_t result = (in_value >> 4) & MAX_8BIT;
    return ~result;
}

uint16_t GunIOUtils::CONVERT_16bit_to_10bitLE(uint16_t in_value) {
    return __bswap_constant_16(in_value % MAX_10BIT);
}
