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
// #pragma once

#include "yivo/yivopkt.h"
#include <stdbool.h>
#include <stdint.h>
#include <string.h> // memcpy, memset

typedef struct {
  uint16_t payload_size; // payload size doesn't count header/cs (+6 bytes more)
  uint8_t readState;     // parser state
  uint8_t buffer_msgid;  // detected message id
} parse_state_t;

typedef struct {
  uint8_t *buffer;
  uint32_t buffer_size;
  uint32_t p; // index of buffer
  parse_state_t state;
} yivo_parser_t;

bool yivo_parse_init(yivo_parser_t *y, uint32_t size);
uint8_t yivo_parse(yivo_parser_t *y, uint8_t c);
yivopkt_t *yivo_parse_get(yivo_parser_t *y);

// class Parser {
// public:
//   Parser(size_t size=64): reserve_size(size) {
//     readState = NONE_STATE;
//     buffer.reserve(size);
//   }

//   ~Parser() {}

//   void get_packet(yivopkt_t& p) {
//     p.fill(buffer.data(), buffer.size());
//   }

//
// Read in one byte at a time, yivo keeps track of the state until it
// finds a good message.
// Returns: bool true - valid message, false - no message yet

// protected:
//   uint16_t payload_size; // payload size doesn't count header/cs (+6B more)
//   const size_t reserve_size; // to minimize vector resizes, get a good chunk
//   initially uint8_t readState; uint8_t buffer_msgid; std::vector<uint8_t>
//   buffer; // used for reading in data

//   void reset_buffer() {
//     buffer.clear();
//     buffer.reserve(reserve_size);
//     readState    = NONE_STATE;
//     payload_size = 0;
//     buffer_msgid = 0;
//   }

//   enum ReadState_t {
//     NONE_STATE, // 0
//     H0_STATE,   // 1
//     H1_STATE,   // 2
//     S0_STATE,   // 3
//     S1_STATE,   // 4
//     TYPE_STATE, // 5
//     DATA_STATE, // 6
//     CS_STATE    // 7
//   };

// }; // end class

// } // end namespace