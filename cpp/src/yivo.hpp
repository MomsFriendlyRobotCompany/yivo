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
#include <cstring>
#include <vector>
#include <numeric> // std::accumulate
#include<iostream>
// using std::cout;
// using std::endl;

union int_t {
  uint16_t b16;
  uint8_t b8[2];
};


enum Error : uint8_t {
  NONE             = 0,
  INVALID_HEADER   = 1,
  INVALID_LENGTH   = 2,
  INVALID_CHECKSUM = 4,
  INVALID_COMMAND  = 8,
  INVALID_MSGID    = 16,
  EXCEED_BUFFER    = 32
};

using YivoPack_t = std::vector<uint8_t>;

// static
// message_t &operator<<(YivoPack_t &msg, const std::string& s) {
//   for (int i=0; i<s.size(); ++i) msg.push_back((uint8_t)s[i]);
//   return msg;
// }
static
std::string to_string(const YivoPack_t& msg) {
  std::string s;
  if (msg.size() == 0) return s;
  return std::accumulate(msg.begin()+1, msg.end(), std::to_string(int(msg[0])),
    [](const std::string& a, uint8_t b) {
      return a + "," + std::to_string(int(b));
    }
  );
}

static
std::ostream &operator<<(std::ostream &os, YivoPack_t const &msg) {
  return os << to_string(msg);
}

static
uint8_t checksum(uint16_t payload_size, uint8_t msgid, uint8_t *data) {
  uint8_t cs = 0;
  // uint8_t hb = uint8_t(size >> 8);
  // uint8_t lb = uint8_t(size & 0xFFFF);
  // cs = lb ^ hb;
  int_t v;
  v.b16 = payload_size;
  cs    = v.b8[0] ^ v.b8[1];
  cs ^= msgid;
  for (int i = 0; i < payload_size; ++i) {
    cs ^= data[i];
  }

  return cs;
}

constexpr uint16_t YIVO_BUFFER_SIZE = 128;
constexpr uint8_t YIVO_FAIL = 0;

class Yivo {
public:
  Yivo(char a = '$', char b = 'K')
      : h0(a), h1(b), readState(NONE_STATE), error_msg(NONE) {}
  ~Yivo() {}

  /*
  msgid: message ID (0-255)
  data: buffer containing data to be sent
  sz: size of data to be sent, sz < (BUFFER_SIZE - 6)
  */
  YivoPack_t pack(uint8_t msgid, uint8_t *data, uint16_t sz) {
    YivoPack_t ret(sz + 6, 0);

    ret[0] = h0;
    ret[1] = h1;
    ret[2] = uint8_t(sz & 0xFF); // low byte
    ret[3] = uint8_t(sz >> 8);   // high byte
    ret[4] = msgid;

    uint8_t cs;
    if (sz > 0) {
      memcpy(&ret[5], data, sz);
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
  // inline
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
    // for (int i=0; i < (payload_size+6); ++i) std::cout << int(buff[i]) << ",";
    // std::cout << std::endl;
    return unpack<T>(buff, payload_size+6);
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
  uint8_t read(uint8_t c) {
    uint8_t ret = YIVO_FAIL;
    switch (readState) {
    case NONE_STATE:
      if (c == this->h0) {
        reset_buffer();
        readState = H0_STATE;
        buff[0]   = c; // h0
        error_msg = NONE;
      }
      else {
        error_msg = INVALID_HEADER;
      }
      break;
    case H0_STATE:
      if (c == this->h1) {
        readState = H1_STATE;
        buff[1]   = c; // h1
      }
      else {
        readState = NONE_STATE;
        index     = 0;
        buff[0]   = 0;
        error_msg = INVALID_HEADER;
      }
      break;
    case H1_STATE:
      readState = S0_STATE;
      buff[2]   = c; // size0
      break;
    case S0_STATE:
      readState    = S1_STATE;
      payload_size = (c << 8) | buff[2];
      // cout << "size: " << int(buff[2]) << " " << int(c) << " " <<
      // payload_size << endl;
      buff[3] = c; // size1
      break;
    case S1_STATE:
      readState    = TYPE_STATE;
      buff[4]      = c; // type or msg id
      buffer_msgid = c;
      break;
    case TYPE_STATE:
      readState = DATA_STATE;
      buff[5]   = c; // data0
      break;
    case DATA_STATE:
      buff[index++] = c; // data1-dataN
      if ((index - 5) == (payload_size)) readState = CS_STATE;
      break;
    case CS_STATE:
      buff[index] = c; // checksum
      // check if cs is correct
      uint8_t cs = checksum(payload_size, buffer_msgid, &buff[5]);
      if (cs == c) ret = buffer_msgid;
      readState = NONE_STATE;
      break;
    }

    return ret;
  }

  // inline uint8_t *get_buffer() { return this->buff; }

  // inline uint8_t *get_payload_buffer() { return &this->buff[5]; }

  // inline uint16_t get_total_size() const { return this->payload_size + 6; }

  // inline uint16_t get_payload_size() const { return this->payload_size; }

  // inline const uint8_t get_buffer_msgid() const { return buffer_msgid; }

  inline const uint8_t get_error_msg() const { return error_msg; }

protected:
  uint8_t error_msg;
  uint8_t buff[YIVO_BUFFER_SIZE];
  uint16_t payload_size; // payload size, doesn't count header/cs (+6B more)
  const char h0, h1;     // header bytes

  uint16_t index;
  uint8_t readState;
  uint8_t buffer_msgid;

  void reset_buffer() {
    memset(buff, 0, YIVO_BUFFER_SIZE);
    readState    = NONE_STATE;
    payload_size = 0;
    index        = 6;
    buffer_msgid = 0;
  }
};



// #ifdef Arduino_h
//     // inline int pack_n_send(uint8_t msgid) {
//     //     // return pack_n_send(msgid, nullptr, 0);
//     //     return pack_n_send(msgid, this->buff, this->payload_size);
//     // }

//     int pack_n_send(uint8_t msgid, uint8_t *data, uint16_t sz) {
//       int size = pack(msgid, data, sz);
//       if (size != -1) {
//         Serial.write(buff, size);
//       }
//       return size;
//     }

//     /*
//     Reads in a packet from a serial connection.

//     Returns: message ID or -1 on error
//     */
//     int read_packet() {
//       int retry = 16;
//       while (retry-- > 0) {
//         if (Serial.available() > 5) {
//           buff[0] = buff[1];
//           buff[1] = Serial.read();
//           if ((buff[0] == this->h0) && (buff[1] == this->h1)) {
//             buff[2] = Serial.read(); // low byte
//             buff[3] = Serial.read(); // high byte
//             uint16_t payload_size = (buff[3] << 8) + buff[2];
//             buff[4] = Serial.read(); // msgid
//             Serial.readBytes(reinterpret_cast<char*>(&buff[5]),
//             payload_size); // payload buff[5+payload_size] = Serial.read();
//             // checksum return buff[4]; // msgid
//           }
//         }
//         else delay(100);
//       }

//       return -1;
//     }
// #else
//     int read_packet() {
//         return -1;
//     }

//     inline int pack_n_send(uint8_t msgid, uint8_t *data=nullptr, uint16_t
//     sz=0) {
//         return -1;
//     }
// #endif