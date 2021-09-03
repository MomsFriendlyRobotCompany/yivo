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
#include <string>
#include <stdint.h>
// #include <strings.h> // memset
// #include <exception>

#define HEADER   255
#define IMU_MSG    2
#define PT_MSG     4
#define LIGHT_MSG  8
#define IR_MSG    16

/*
Packet construction class.

b sint8
B uint8
h sint16
H uint16
i sint32
I uint32
f float[4]
d double[8]
*/

class Packet {
public:
    /*
    [0xff,0xff,id,len,msb,...,lsb,chksum]
    id: message number
    len: number of parameters, no counting header(2), id(1), len(1), checksum(1)
    */
    Packet(uint8_t id, uint8_t len);
    Packet(uint8_t id, const std::string& fmt);

    int size();
    // void push(const int& a);
    // void push(const char& a);
    void push(const uint8_t& a);
    void push(const int8_t& a);
    void push(uint16_t a);
    void push(int16_t a);
    void push(uint32_t a);
    void push(int32_t a);
    void push(float f);
    void end();
    uint8_t compute_checksum();
    // bool valid_checksum(const std::vector<uint8_t>& v);

    static bool valid_checksum(const std::vector<uint8_t>& v){
        uint32_t checksum = 0;
        size_t len = v.size()-1;
        for (uint8_t i = 2; i < len; ++i) checksum += v[i];
        uint8_t ans = (~checksum) & 0xFF;
        return ans == v[len];
    }

    std::vector<uint8_t> pkt;
};

/*
Base class for other messages.
*/
class SerialLib {
public:
    float to_float(const std::vector<uint8_t>& p, int i);
    uint32_t to_uint32(const std::vector<uint8_t>& p, int i);
    int32_t to_int32(const std::vector<uint8_t>& p, int i);
    uint16_t to_uint16(const std::vector<uint8_t>& p, int i);
    int16_t to_int16(const std::vector<uint8_t>& p, int i);
};
//
// /*
// 9DOF Inertial Measurement Unit sensor message
// */
// class IMU: public SerialLib {
// public:
//     IMU();
//
//     void accels(const float& x, const float& y, const float& z);
//     void gyros(const float& x, const float& y, const float& z);
//     void mags(const float& x, const float& y, const float& z);
//
//     const Packet serialize();
//     void deserialize(const std::vector<uint8_t>& p);
//
//     float ax,ay,az; // 4*3 = 12
//     float gx,gy,gz; // 12
//     float mx,my,mz; // 12
//     float temp;     // 4
//                     // params = 40
//                     // total = 45
// };
//
// /*
// Pressure and temperature sensor message.
// */
// class PT: public SerialLib {
// public:
//     PT();
//
//     const Packet serialize();
//     void deserialize(const std::vector<uint8_t>& p);
//
//     float pressure, temperature;
// };
