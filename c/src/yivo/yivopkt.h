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

typedef struct {
  uint16_t size; // entire message: payload_size + header
  uint8_t *data; // entire message: header + payload
  uint8_t msg_id;
} ypkt_t;

// bool ypkt_init(ypkt_t *y, uint16_t size);
// ypkt_t *ypkt_init(uint16_t size);
ypkt_t *ypkt_create(const uint8_t msgid, uint16_t size);
ypkt_t *ypkt_free(ypkt_t *y);
int ypkt_pack(ypkt_t *y, uint8_t *data, const uint16_t len);
int ypkt_unpack(ypkt_t *y, void *dst, uint16_t size);
int ypkt_valid_msg(ypkt_t *y);

extern const uint8_t crc8_table[256];
