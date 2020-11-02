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
#include <iostream>

using namespace std;

Packet::Packet(uint8_t id, uint8_t len){
    pkt.reserve(len+5);
    // 0xff,0xff,id,len
    pkt.push_back(0xff);
    pkt.push_back(0xff);
    pkt.push_back(id);
    pkt.push_back(len+1); // params + chksum
}

Packet::Packet(uint8_t id, const std::string& fmt){
    int len = 0;

    for (auto const& c: fmt){
        if (c == 'b' || c == 'B') len +=1;                  // int8
        else if (c == 'h' || c == 'H') len +=2;             // int16
        else if (c == 'i' || c == 'I' || c == 'f') len +=4; // int32 or float
        else if (c == 'd') len +=8;                         // double
    }

    cout << ">> Length: " << len << endl;

    pkt.reserve(len+5);
    // 0xff,0xff,id,len
    pkt.push_back(0xff);
    pkt.push_back(0xff);
    pkt.push_back(id);
    pkt.push_back(len+1); // params + chksum
}

int Packet::size(){return pkt.size();}

// void Packet::push(const int& a){
//     pkt.push_back(static_cast<uint8_t>(a));
// }
//
// void Packet::push(const char& a){
//     pkt.push_back(static_cast<uint8_t>(a));
// }

void Packet::push(const uint8_t& a){
    pkt.push_back(a);
}

void Packet::push(const int8_t& a){
    pkt.push_back(a);
}

void Packet::push(int16_t a){
    uint8_t *c = reinterpret_cast<uint8_t*>(&a);
    pkt.push_back(c[1]); // MSB
    pkt.push_back(c[0]); // LSB
}

void Packet::push(uint16_t a){
    uint8_t *c = reinterpret_cast<uint8_t*>(&a);
    pkt.push_back(c[1]); // MSB
    pkt.push_back(c[0]); // LSB
}

void Packet::push(uint32_t a){
    uint8_t *c = reinterpret_cast<uint8_t*>(&a);
    pkt.push_back(c[3]); // MSB
    pkt.push_back(c[2]);
    pkt.push_back(c[1]);
    pkt.push_back(c[0]); // LSB
}

void Packet::push(int32_t a){
    uint8_t *c = reinterpret_cast<uint8_t*>(&a);
    pkt.push_back(c[3]); // MSB
    pkt.push_back(c[2]);
    pkt.push_back(c[1]);
    pkt.push_back(c[0]); // LSB
}

void Packet::push(float f){
    // cout << "float: " << f << endl;
    uint8_t *c = reinterpret_cast<uint8_t*>(&f);
    pkt.push_back(c[3]); // MSB
    pkt.push_back(c[2]);
    pkt.push_back(c[1]);
    pkt.push_back(c[0]); // LSB
}

void Packet::end(){
    uint8_t chksum = compute_checksum();
    pkt.push_back(chksum);
}

uint8_t Packet::compute_checksum(){
    uint32_t checksum = 0;
    for (uint8_t i = 4; i < pkt.size(); ++i) checksum += pkt[i];
    return (~checksum) & 0xFF;
}

// bool Packet::valid_checksum(const std::vector<uint8_t>& v){
//     uint32_t checksum = 0;
//     size_t len = v.size()-1;
//     for (uint8_t i = 2; i < len; ++i) checksum += v[i];
//     uint8_t ans = (~checksum) & 0xFF;
//     return ans == v[len];
// }
