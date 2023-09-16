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
#include <ostream>
#include <stdint.h>

namespace HIDDEN {
union int_t {
  uint16_t b16;
  uint8_t b8[2];
};
} // hidden

// using YivoPack_t = std::vector<uint8_t>;
class YivoPack_t: public std::vector<uint8_t> {
  public:
  uint8_t msgid() { return (*this)[4]; }
  uint16_t playload_size() { return ((*this)[3] << 8) | (*this)[2];}
  // uint16_t playload_size() { return size() - 6;}
};

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
  HIDDEN::int_t v;
  v.b16 = payload_size;
  cs    = v.b8[0] ^ v.b8[1];
  cs ^= msgid;
  for (int i = 0; i < payload_size; ++i) {
    cs ^= data[i];
  }

  return cs;
}