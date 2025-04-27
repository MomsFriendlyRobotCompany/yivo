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

#include <stdbool.h>
#include <stdint.h>

#define YIVO_HEADER_0 '$'
#define YIVO_HEADER_1 'K'
#define YIVO_H0 0
#define YIVO_H1 1
#define YIVO_LN 2
#define YIVO_HN 3
#define YIVO_ID 4
#define YIVO_PL 5
#define YIVO_OVERHEAD 6 // h0,h1,LN,HN,ID, ..., CS

#define YIVO_PTR_NULL -1
#define YIVO_BUFFER_NULL -2
#define YIVO_INVALID_HEADER -3
#define YIVO_INVALID_CHECKSUM -4
#define YIVO_EXCEED_BUFFER_SIZE -5
// #define YIVO_PTR_NULL -1
// #define YIVO_PTR_NULL -1

typedef struct {
  uint8_t *buffer;
  uint16_t buffer_size; // payload_size + 6
} yivopkt_t;

// bool yivo_init(yivopkt_t *y, uint16_t size);
yivopkt_t *yivo_init(uint16_t size);
bool yivo_free(yivopkt_t *y);
bool yivo_pack(yivopkt_t *y, const uint8_t msgid, uint8_t *data,
               const uint16_t len);
int32_t yivo_unpack(yivopkt_t *y, void *dst);
inline uint8_t yivo_msg_id(yivopkt_t *y) { return y->buffer[YIVO_ID]; }
inline uint16_t yivo_payload_size(yivopkt_t *y) {
  return ((uint16_t)y->buffer[YIVO_HN] << 8) | y->buffer[YIVO_LN];
}
bool yivo_valid_msg(yivopkt_t *y);
