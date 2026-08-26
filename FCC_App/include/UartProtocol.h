#pragma once
#include <stdint.h>

// CRC8/SMBUS (poly=0x07, init=0x00)
static inline uint8_t uart_crc8(const uint8_t* data, uint8_t len) {
    uint8_t crc = 0x00u;
    for (uint8_t i = 0; i < len; i++) {
        crc ^= data[i];
        for (uint8_t j = 0; j < 8; j++)
            crc = (crc & 0x80u) ? (uint8_t)((crc << 1) ^ 0x07u) : (uint8_t)(crc << 1);
    }
    return crc;
}

// String frame format (human-readable, debuggable in PuTTY):
// FCC -> STM32:  "HB:<SEQ4HEX>:<CRC2HEX>\n"      e.g. "HB:0001:A3\n"
// STM32 -> FCC:  "ACK:211:<SEQ4HEX>:<CRC2HEX>\n"  e.g. "ACK:211:0001:B5\n"
// CRC covers all bytes up to and including the last ':' before the CRC field.
