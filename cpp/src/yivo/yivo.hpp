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
// #include <stdio.h>
// #include <map>

constexpr int BUFFER_SIZE = 256;

// Message IDs
constexpr int IMU_AGMQT = 140;
constexpr int IMU_AGMT  = 141;
constexpr int IMU_AGT   = 142;
constexpr int IMU_AT    = 143;
constexpr int MAGNETIC  = 144;
constexpr int RANGE     = 145;


enum Error {
    NONE = 0,
    INVALID_HEADER   = 1,
    INVALID_LENGTH   = 2,
    INVALID_CHECKSUM = 4,
    INVALID_COMMAND  = 8,
    INVALID_MSGID    = 16,
    EXCEED_BUFFER    = 32
};

// std::map<int,

uint8_t checksum(uint16_t size, uint8_t msgid, uint8_t *data){
    uint8_t cs = 0;
    uint8_t hb = uint8_t(size >> 8);
    uint8_t lb = uint8_t(size & 0xFF);
    cs = lb ^ hb;
    cs ^= msgid;
    int sz = (hb << 8) + lb;
    for (int i=0; i < sz; ++i) {
        cs ^= data[i];
    }

    return cs;
}

// struct msg_t {
//     uint8_t msgid;
//     uint16_t size;
//     uint8_t *payload;
//     // uint8_t cs;
// };

// struct base_t {
//     float timestamp;
// };

struct ImuA_t {
    float ax, ay, az;
    // float temperature;
    // float timestamp;
};
struct ImuAGT_t {
    float ax, ay, az;
    float gx, gy, gz;
    float temperature;
    float timestamp;
};
struct ImuAGMT_t {
    float ax, ay, az;
    float gx, gy, gz;
    float mx, my, mz;
    float temperature;
    float timestamp;
};
struct ImuAGMQT_t {
    float ax, ay, az;
    float gx, gy, gz;
    float mx, my, mz;
    float qw,qx,qy,qz;
    float temperature;
    float timestamp;
};

// template<typename T>
class Yivo {
    public:
    Yivo(char a='$', char b='K'): h0(a), h1(b) {}
    ~Yivo() {}

    Error pack(uint8_t msgid, uint8_t *data, uint16_t size) {
        if (size > BUFFER_SIZE) {
            // ??
            return Error::EXCEED_BUFFER;
        }

        memset(this->buff, 0, BUFFER_SIZE);
        this->buff[0] = h0;
        this->buff[1] = h1;
        this->buff[2] = uint8_t(size >> 8);
        this->buff[3] = uint8_t(size & 0xFF);
        this->buff[4] = msgid;
        memcpy(&this->buff[5], data, size);

        uint8_t cs = checksum(size, msgid, data);
        // printf("cs: %d\n",int(cs));
        this->buff[5+size] = cs; // off by 1?

        this->size = size;
        return Error::NONE;
    }

    template<typename T>
    T unpack(uint8_t *payload, uint16_t payload_size) {
        T val;
        memcpy(&val, payload, payload_size);
        return val;
    }

    template<typename T>
    T unpack() {
        T val;
        memcpy(&val, this->get_payload_buffer(), this->get_payload_size());
        return val;
    }

    // uint8_t unpack2()

    // msg_t read_packet() {
    //     msg_t msg{0,0,nullptr}; // size, id, data

    //     return msg;
    // }

    uint8_t read_packet() {
        return 1;
    }

    inline uint8_t* get_buffer() {
        return this->buff;
    }

    inline uint8_t* get_payload_buffer() {
        return &this->buff[5];
    }

    inline uint16_t get_total_size() const {
        return this->size + 6;
    }

    inline uint16_t get_payload_size() const {
        return this->size;
    }

    protected:
    uint8_t buff[BUFFER_SIZE];
    uint16_t size;
    char h0, h1;
};