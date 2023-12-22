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


namespace yivo {

constexpr uint8_t YIVO_HEADER_0 = '$';
constexpr uint8_t YIVO_HEADER_1 = 'K';
constexpr uint16_t YIVO_H0 = 0;
constexpr uint16_t YIVO_H1 = 1;
constexpr uint16_t YIVO_LN = 2;
constexpr uint16_t YIVO_HN = 3;
constexpr uint16_t YIVO_ID = 4;
constexpr uint16_t YIVO_PL = 5;
constexpr uint16_t YIVO_OVERHEAD = 6; // h0,h1,LN,HN,ID, ..., CS

class yivopkt_t {
  public:
  yivopkt_t() {}

  yivopkt_t(const yivopkt_t&) = delete;
  yivopkt_t(const yivopkt_t&&) = delete;
  yivopkt_t& operator=(const yivopkt_t&) = delete;

  // yivopkt_t(const yivopkt_t && y) {
  //   resize(y.size());
  //   memcpy(buffer, y.data(), y.size());
  // }

  ~yivopkt_t() {
    if (buffer != nullptr) delete[] buffer;
    // printf("del\n");
  }

  void clear() {
    if (buffer != nullptr) delete[] buffer;
    buffer = nullptr;
    buffer_size = 0;
  }

  /*
  msgid: message ID (1-255), 0 is not allowed
  data: buffer containing payload data to be sent
  sz: size of payload to be sent, yivopkt_t.size() == (payload_size + 6)
  */
  void pack(const uint8_t msgid, uint8_t* data, const uint16_t len) {
    if (data == nullptr) return;
    resize(len+YIVO_OVERHEAD);

    buffer[0] = YIVO_HEADER_0;
    buffer[1] = YIVO_HEADER_1;
    buffer[YIVO_LN] = (uint8_t)(len & 0x00FF); // lo
    buffer[YIVO_HN] = (uint8_t)(len >> 8); // hi
    buffer[YIVO_ID] = msgid;

    memcpy(&buffer[YIVO_PL], data, len);
    buffer[len+5] = calc_checksum();
  }

  /*
  This is like a 'copy' for the whole yivo packet from header0
  to checksum.
  */
  void fill(uint8_t* data, const uint16_t len) {
    if (data == nullptr) return;
    resize(len);
    memcpy(buffer, data, len);
  }

  template <typename T>
  inline
  T unpack() {
    T val{0};
    if (buffer == nullptr || buffer_size < 6) return val;
    // uint16_t len = buffer_size - 6; //(buffer[YIVO_HN] << 8) | buffer[YIVO_LN];
    memcpy((uint8_t *)(&val), &buffer[YIVO_PL], buffer_size - 6);
    return val;
  }

  inline uint16_t size() const { return buffer_size; }
  inline const uint8_t* data() const {return buffer;}
  inline const uint8_t operator[](uint16_t i) const {return buffer[i];}
  inline uint8_t msg_id() { return buffer[YIVO_ID]; }
  inline uint16_t payload_size() { return ((uint16_t)buffer[YIVO_HN] << 8) | buffer[YIVO_LN]; }
  inline uint8_t checksum() { return buffer[buffer_size - 1]; }
  inline bool has_valid_checksum() { return calc_checksum() == checksum(); }

  bool valid_msg() {
    if (buffer == nullptr) return false;
    if (buffer_size < 6) return false;
    if ((buffer[0] != YIVO_HEADER_0) || (buffer[1] != YIVO_HEADER_1)) return false;
    if ((buffer_size - 6) != ((buffer[YIVO_HN] << 8) | buffer[YIVO_LN])) return false;
    if (buffer[YIVO_ID] == 0) return false;
    return has_valid_checksum();
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
    // XOR all bytes EXCEPT header and checksum
    uint32_t cs = 0;
    for (uint16_t i = 2; i < (buffer_size - 1); ++i) cs ^= buffer[i];
    return (uint8_t)(cs & 0x000000FF);
  }

  uint8_t* buffer=nullptr;
  uint16_t buffer_size{0}; // payload_size + 6

}; // end class

} // end namespace
