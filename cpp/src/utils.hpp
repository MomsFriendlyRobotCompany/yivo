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

#include <ostream>
#include <numeric> // std::accumulate
#include <string>

namespace yv {
static
std::string to_string(const yivopkt_t& msg) {
  std::string s;
  if (msg.size() == 0) return s;
  s += (char)msg[0];
  s += ',';
  s += (char)msg[1];
  return std::accumulate(msg.begin()+2, msg.end(), s,
    [](const std::string& a, uint8_t b) {
      return a + "," + std::to_string(int(b));
    }
  );
  return s;
}
}

inline
std::ostream &operator<<(std::ostream &os, yivopkt_t const &msg) {
  return os << yv::to_string(msg);
}
