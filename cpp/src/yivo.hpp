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

#include <stdint.h>
#include <cstring> // std::memcpy
#include <vector>
#include <numeric> // std::accumulate
#include "yivopack.hpp"

constexpr uint8_t YIVO_NO_ID = 0;

class Yivo {
public:
  Yivo(size_t size=64): reserve_size(size) {
    h0 = '$';
    h1 = 'K';
    readState = NONE_STATE;
    buffer.reserve(size);
  }

  ~Yivo() {}

  void set_header(char hdr0, char hdr1) {
     h0 = hdr0;
     h1 = hdr1;
  }

  /*
  msgid: message ID (1-255), 0 is not allowed
  data: buffer containing payload data to be sent
  sz: size of payload to be sent, YivoPack_t.size() == (payload_size + 6)
  */
  YivoPack_t pack(uint8_t msgid, uint8_t *data, uint16_t sz) {
    YivoPack_t ret;
    ret.resize(sz + 6, 0);

    ret[0] = h0;
    ret[1] = h1;
    ret[2] = uint8_t(sz & 0xFF); // low byte
    ret[3] = uint8_t(sz >> 8);   // high byte
    ret[4] = msgid;

    uint8_t cs;
    if (sz > 0) {
      uint8_t *ptr = ret.data();
      std::memcpy(&ptr[5], data, sz);
      cs = checksum(sz, msgid, data);
    }
    else cs = msgid;
    ret[5 + sz] = cs;
    this->payload_size = sz;

    return ret;
  }

  bool valid_msg(uint8_t *buf, uint16_t size) {
    uint16_t pl_size = size - 6;
    if (buf == nullptr) return false;
    if ((buf[0] != h0) || (buf[1] != h1)) return false;
    uint16_t payload_len     = (buf[3] << 8) | buf[2];

    // std::cout << char(buf[0]) << " " << char(buf[1]) << std::endl;
    // std::cout << int(len) << " " << int(size) << std::endl;

    if (payload_len != pl_size) return false;
    uint8_t msgid    = buf[4];
    uint8_t *payload = &buf[5];
    uint8_t cs       = checksum(payload_len, msgid, payload);
    // std::cout << int(cs) << " " << int(buf[size-1]) << std::endl;
    if (cs != buf[size-1]) return false;
    return true;
  }

  inline
  bool valid_msg(YivoPack_t& msg) {
    return valid_msg(msg.data(), msg.size());
  }

  template <typename T>
  T unpack(uint8_t* b, size_t size) {
    T val{0};
    if (b == nullptr) return val;
    if (!valid_msg(b, size)) return val;
    uint16_t len = (b[3] << 8) | b[2];
    memcpy((void *)(&val), (void *)(&b[5]), len);
    return val;
  }

  template <typename T>
  inline
  T unpack(YivoPack_t& msg) {
    return unpack<T>(msg.data(), msg.size());
  }

  template <typename T>
  inline
  T unpack() {
    // return unpack<T>(buffer.data(), payload_size+6);
    return unpack<T>(buffer.data(), buffer.size());
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

  /*
  Read in one byte at a time, yivo keeps track of the state until it
  finds a good message.
  Returns: bool true - valid message, false - no message yet
  */
  uint8_t parse(uint8_t c) {
    uint8_t ret = YIVO_NO_ID;
    switch (readState) {
    case NONE_STATE:
      if (c == this->h0) {
        reset_buffer();
        readState = H0_STATE;
        buffer.push_back(c); // h0
      }
      else readState = NONE_STATE; // this state will reset
      break;
    case H0_STATE:
      if (c == this->h1) {
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
      uint8_t cs = checksum(payload_size, buffer_msgid, &buffer[5]);
      if (cs == c) ret = buffer_msgid;
      readState = NONE_STATE;
      break;
    }

    return ret;
  }

  [[deprecated]]
  uint8_t read(uint8_t c) {
    return parse(c);
  }

protected:
  uint16_t payload_size; // payload size, doesn't count header/cs (+6B more)
  char h0, h1;           // header bytes
  const size_t reserve_size;
  uint8_t readState;
  uint8_t buffer_msgid;
  std::vector<uint8_t> buffer;

  void reset_buffer() {
    buffer.clear();
    buffer.reserve(reserve_size);
    readState    = NONE_STATE;
    payload_size = 0;
    buffer_msgid = YIVO_NO_ID;
  }
};
