// /******************************************************************************
// MIT License

// Copyright (c) 2020 Mom's Friendly Robot Company

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
// ******************************************************************************/
#pragma once

#include "yivo/yivopkt.h"
#include <stdbool.h>
#include <stdint.h>
#include <string.h> // memcpy, memset

typedef struct {
  uint8_t *payload;      // payload size doesn't count header (+6 bytes more)
  uint16_t payload_size; // size of payload
  uint8_t readState;     // parser state
  uint8_t buffer_msgid;  // detected message id
  uint16_t index;        // index of buffer
  uint8_t cs;            // checksum
} yivo_parser_t;

yivo_parser_t *yivo_parse_create();
void yivo_parse_reset(yivo_parser_t *y);
uint8_t yivo_parse(yivo_parser_t *y, uint8_t c);
int32_t yivo_parse_get(yivo_parser_t *y, uint8_t *buffer, uint16_t size);

// bool yivo_parse_init(yivo_parser_t *y);