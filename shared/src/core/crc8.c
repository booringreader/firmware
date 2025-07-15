#include "crc8.h"

uint8_t crc8(uint8_t* data, uint32_t length){
    uint8_t crc = 0;
    
    for(uint32_t i = 0; i < length; i++) {
        crc ^= data[i]; // XOR the byte with the current CRC value
        for (uint8_t j = 0; j < 8; j++) {
            if (crc & 0x80) { // If the highest bit is set
                crc = (crc << 1) ^ 0x07; // Shift left and XOR with polynomial
            } else {
                crc <<= 1; // Just shift left
            }
        }
    }
    return crc;
}