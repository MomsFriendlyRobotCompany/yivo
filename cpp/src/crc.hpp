
#pragma once

#include <cstdint>

// // CRC-8 polynomial (0x07)
// #define CRC8_POLYNOMIAL 0x07

// // Function to calculate CRC-8 checksum
// uint8_t crc8(const uint8_t *data, size_t length) {
//     uint8_t crc = 0;

//     for (size_t i = 0; i < length; ++i) {
//         crc ^= data[i];

//         for (int j = 0; j < 8; ++j) {
//             crc = (crc & 0x80) ? (crc << 1) ^ CRC8_POLYNOMIAL : (crc << 1);
//         }
//     }

//     return crc;
// }

// // CRC-16 polynomial (0x8005)
// #define CRC16_POLYNOMIAL 0x8005

// // Function to calculate CRC-16 checksum
// uint16_t crc16(const uint8_t *data, size_t length) {
//     uint16_t crc = 0xFFFF;

//     for (size_t i = 0; i < length; ++i) {
//         crc ^= (uint16_t)data[i];

//         for (int j = 0; j < 8; ++j) {
//             crc = (crc & 0x0001) ? (crc >> 1) ^ CRC16_POLYNOMIAL : (crc >> 1);
//         }
//     }

//     return crc;
// }