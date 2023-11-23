/******************************************************************************
MIT License

Copyright (c) 2020 Mom's Friendly Robot Company

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
******************************************************************************/
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