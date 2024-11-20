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
#include <cstring> // std::memcpy
#include <vector>
#include <vector>
#include "yivopkt.hpp"


namespace yivo {


// enum ReadState_t {
//   NONE_STATE, // 0
//   H0_STATE,   // 1
//   H1_STATE,   // 2
//   S0_STATE,   // 3
//   S1_STATE,   // 4
//   TYPE_STATE, // 5
//   DATA_STATE, // 6
//   CS_STATE    // 7
// };


// uint8_t parse(uint8_t *inbuffer, size_t size) {
//   uint8_t ret = 0;
//   uint8_t c;
//   uint8_t buffer_msgid = 0;
//   uint8_t readState = NONE_STATE;
//   uint16_t payload_size = 0;
  
//   for (size_t i=0; i < size; ++i) {
//     c = buffer[i];
//     switch (readState) {
//     case NONE_STATE:
//       if (c == YIVO_HEADER_0) {
//         // reset_buffer();
//         readState = H0_STATE;
//         buffer.push_back(c); // h0
//       }
//       else readState = NONE_STATE; // this state will reset
//       break;
//     case H0_STATE:
//       if (c == YIVO_HEADER_1) {
//         readState = H1_STATE;
//         buffer.push_back(c); // h1
//       }
//       else readState = NONE_STATE; // this state will reset
//       break;
//     case H1_STATE:
//       readState = S0_STATE;
//       buffer.push_back(c); // size0
//       break;
//     case S0_STATE:
//       readState    = S1_STATE;
//       payload_size = (c << 8) | buffer[2];
//       buffer.push_back(c); // size1
//       break;
//     case S1_STATE:
//       readState    = TYPE_STATE;
//       buffer.push_back(c); // type or msg id
//       buffer_msgid = c;
//       break;
//     case TYPE_STATE:
//       readState = DATA_STATE;
//       buffer.push_back(c); // data0
//       break;
//     case DATA_STATE:
//       buffer.push_back(c); // data1-dataN
//       if ((buffer.size() - 5) == payload_size) readState = CS_STATE;
//       break;
//     case CS_STATE:
//       buffer.push_back(c); // checksum
//       // check if cs is correct
//       uint8_t cs = 0;
//       for (size_t i=2; i < buffer.size()-1; ++i) cs ^= buffer[i];
//       if (cs == c) {
//         ret = buffer_msgid;
//         // printf("good checksum\n");
//         break;
//       }
//       // else printf(" %d != %d\n", (int)c, (int)cs);
//       readState = NONE_STATE;
//       break;
//     }
//   }

//   return ret;
// }

class Parser {
public:
  Parser(size_t size=64): reserve_size(size) {
    readState = NONE_STATE;
    buffer.reserve(size);
  }

  ~Parser() {}

  void get_packet(yivopkt_t& p) {
    p.fill(buffer.data(), buffer.size());
  }

  /*
  Read in one byte at a time, yivo keeps track of the state until it
  finds a good message.
  Returns: bool true - valid message, false - no message yet
  */
  uint8_t parse(uint8_t c) {
    uint8_t ret = 0;
    switch (readState) {
    case NONE_STATE:
      if (c == YIVO_HEADER_0) {
        reset_buffer();
        readState = H0_STATE;
        buffer.push_back(c); // h0
      }
      else readState = NONE_STATE; // this state will reset
      break;
    case H0_STATE:
      if (c == YIVO_HEADER_1) {
        readState = H1_STATE;
        buffer.push_back(c); // h1
      }
      else readState = NONE_STATE; // this state will reset
      break;
    case H1_STATE:
      readState = S0_STATE;
      buffer.push_back(c); // size0
      break;
    case S0_STATE:
      readState    = S1_STATE;
      payload_size = (c << 8) | buffer[2];
      buffer.push_back(c); // size1
      break;
    case S1_STATE:
      readState    = TYPE_STATE;
      buffer.push_back(c); // type or msg id
      buffer_msgid = c;
      break;
    case TYPE_STATE:
      readState = DATA_STATE;
      buffer.push_back(c); // data0
      break;
    case DATA_STATE:
      buffer.push_back(c); // data1-dataN
      if ((buffer.size() - 5) == payload_size) readState = CS_STATE;
      break;
    case CS_STATE:
      buffer.push_back(c); // checksum
      // check if cs is correct
      uint8_t cs = 0;
      for (size_t i=2; i < buffer.size()-1; ++i) cs ^= buffer[i];
      if (cs == c) {
        ret = buffer_msgid;
        // printf("good checksum\n");
      }
      // else printf(" %d != %d\n", (int)c, (int)cs);
      readState = NONE_STATE;
      break;
    }

    return ret;
  }

protected:
  uint16_t payload_size; // payload size doesn't count header/cs (+6B more)
  const size_t reserve_size; // to minimize vector resizes, get a good chunk initially
  uint8_t readState;
  uint8_t buffer_msgid;
  std::vector<uint8_t> buffer; // used for reading in data

  void reset_buffer() {
    buffer.clear();
    buffer.reserve(reserve_size);
    readState    = NONE_STATE;
    payload_size = 0;
    buffer_msgid = 0;
  }

  enum ReadState_t {
    NONE_STATE, // 0
    H0_STATE,   // 1
    H1_STATE,   // 2
    S0_STATE,   // 3
    S1_STATE,   // 4
    TYPE_STATE, // 5
    DATA_STATE, // 6
    CS_STATE    // 7
  };

}; // end class

} // end namespace