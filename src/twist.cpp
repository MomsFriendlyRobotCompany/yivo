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
#include <yivo/yivo.h>
#include <exception>
#include <stdexcept>

Twist::Twist(){}

const Packet Twist::serialize(){

    Packet pkt(TWIST_MSG,2*4);
    // 0xff
    // 0xff
    // id
    // len
    // pkt.push(pressure);    // 4
    // pkt.push(temperature); // 8
    // pkt.end();

    return pkt;
}
void Twist::deserialize(const std::vector<uint8_t>& p){
    if (p[0] != 0xff || p[1] != 0xff || p[2] != TWIST_MSG) throw std::runtime_error("invalide packet");
    if (!Packet::valid_checksum(p)) throw std::runtime_error("invalide packet");

    // pressure = to_float(p, 4);
    // temperature = to_float(p, 8);
}
