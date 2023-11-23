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
#include <vector>
#include <cstdint>

// constexpr uint16_t YIVO_H0 = 0;
// constexpr uint16_t YIVO_H1 = 1;
constexpr uint16_t YIVO_LN = 2;
constexpr uint16_t YIVO_HN = 3;
constexpr uint16_t YIVO_ID = 4;
constexpr uint16_t YIVO_PL = 5;

class yivopkt_t {
  public:
  yivopkt_t() {}

  yivopkt_t(const yivopkt_t&) = delete;
  // yivopkt_t(const yivopkt_t&&) = delete;
  yivopkt_t& operator=(const yivopkt_t&) = delete;

  yivopkt_t(const yivopkt_t && y) {
    resize(y.size());
    memcpy(buffer, y.data(), y.size());
  }

  ~yivopkt_t() {
    if (buffer != nullptr) delete[] buffer;
    // printf("del\n");
  }

  void pack(uint8_t msgid, uint8_t* data, uint16_t len) {
    if (data == nullptr) return;
    resize(len+6);
    buffer[0] = '$';
    buffer[1] = 'K';
    buffer[YIVO_LN] = (len & 0x00FF); // lo
    buffer[YIVO_HN] = (len >> 8); // hi
    buffer[YIVO_ID] = msgid;
    memcpy(&buffer[YIVO_PL], data, len);
    buffer[len+5] = calc_checksum();
  }

  void fill(uint8_t* data, uint16_t len) {
    if (data == nullptr) return;
    resize(len);
    memcpy(buffer, data, len);
  }

  template <typename T>
  T unpack() {
    T val{0};
    if (buffer == nullptr || buffer_size < 6) return val;
    // uint16_t len = buffer_size - 6; //(buffer[YIVO_HN] << 8) | buffer[YIVO_LN];
    memcpy((void *)(&val), &buffer[YIVO_PL], buffer_size - 6);
    return val;
  }

  inline uint16_t size() const { return buffer_size; }
  inline const uint8_t* data() const {return buffer;}
  inline const uint8_t operator[](uint16_t i) const {return buffer[i];}
  inline uint8_t msg_id() { return buffer[YIVO_ID]; }
  inline uint16_t payload_size() { return ((uint16_t)buffer[YIVO_HN] << 8) | buffer[YIVO_LN]; }
  inline uint8_t checksum() { return buffer[buffer_size - 1]; }
  inline bool has_valid_chksum() { return calc_checksum() == checksum(); }

  bool valid_msg() {
    if (buffer == nullptr) return false;
    if (buffer_size < 6) return false;
    if ((buffer[0] != '$') || (buffer[1] != 'K')) return false;
    if ((buffer_size - 6) != ((buffer[YIVO_HN] << 8) | buffer[YIVO_LN])) return false;
    if (buffer[YIVO_ID] == 0) return false;
    return has_valid_chksum();
  }

  const uint8_t* begin() const { return buffer; }
  const uint8_t* end() const { return buffer+buffer_size; }

  protected:
  void resize(uint16_t s) {
    if (buffer != nullptr) delete [] buffer;
    // printf("new %d\n", (int)s);
    buffer_size = s;
    buffer = new uint8_t[buffer_size];
  }

  uint8_t calc_checksum() {
    uint8_t cs = 0;
    for (uint16_t i = 2; i < (buffer_size - 1); ++i) cs ^= buffer[i];
    return cs;
  }

  uint8_t* buffer=nullptr;
  uint16_t buffer_size{0};

};

// class yivopkt_t: public std::vector<uint8_t> {
//   static constexpr uint8_t h0 = '$';
//   static constexpr uint8_t h1 = 'K';

//   public:
//   inline
//   uint8_t msgid() { return (*this)[4]; }
//   inline
//   uint16_t payload_size() { return ((*this)[3] << 8) | (*this)[2];}
//   inline
//   uint8_t checksum() { return (*this)[size()-1]; }
//   // void set_checksum() { (*this)[size()-1] = this->calc_checksum(); }
//   uint8_t calc_checksum() {
//     uint8_t cs = 0;
//     uint16_t payload_size = this->payload_size();
//     uint8_t *data = this->data();
//     for (uint16_t i = 0; i < payload_size; ++i) {
//       cs ^= data[2+i];
//     }
//     return cs;
//   }

//   inline
//   bool has_valid_chksum() {
//     return this->calc_checksum() == this->checksum();
//   }

//   bool valid_msg() {
//     uint8_t *buf = this->data();
//     if (buf == nullptr) return false;

//     uint16_t size = this->size();
//     if (size < 6) return false;
//     uint16_t payload_len = size - 6;

//     if ((buf[0] != h0) || (buf[1] != h1)) return false;
//     if (payload_len != ((buf[3] << 8) | buf[2])) return false;
//     uint8_t msgid    = buf[4];
//     if (msgid == 0) return false;
//     uint8_t *payload = &buf[5];
//     // uint8_t cs       = checksum(msgid, payload, payload_len);
//     // std::cout << int(cs) << " " << int(buf[size-1]) << std::endl;
//     // if (cs != buf[size-1]) return false;
//     printf("here\n");
//     return has_valid_chksum();
//   }
// };

