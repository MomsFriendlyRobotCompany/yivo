#pragma once

#include <vector>
#include <stdint.h>
#include <strings.h> // memset
#include <exception>

#define HEADER   255
#define IMU_MSG    2
#define PT_MSG     4
#define LIGHT_MSG  8
#define IR_MSG    16

class Packet {
public:
    /*
    [0xff,0xff,id,len,msb,...,lsb,chksum]
    id: message number
    len: number of parameters, no counting header(2), id(1), len(1), checksum(1)
    */
    Packet(uint8_t id, uint8_t len);

    int size();
    void push(const int& a);
    void push(const char& a);
    void push(const uint8_t& a);
    void push(uint16_t a);
    void push(float f);
    void end();
    uint8_t compute_checksum();

    // float to_float(int i);

    static bool valid_checksum(const std::vector<uint8_t>& v){
        uint32_t checksum = 0;
        size_t len = v.size()-1;
        for (uint8_t i = 2; i < len; ++i) checksum += v[i];
        uint8_t ans = (~checksum) & 0xFF;
        return ans == v[len];
    }

    std::vector<uint8_t> pkt;
};

// std::ostream &operator<<(std::ostream &os, Packet const &p) {
//     // for (uint8_t const& u: p.pkt) os << std::hex << int(u) << ",";
//     for (uint8_t const& u: p.pkt) os << int(u) << ",";
//     os << " ";
//     return os;
// }

class SerialLib {
public:
    float to_float(const std::vector<uint8_t>& p, int i);
};

class IMU: public SerialLib {
public:
    IMU();

    void accels(const float& x, const float& y, const float& z);
    void gyros(const float& x, const float& y, const float& z);
    void mags(const float& x, const float& y, const float& z);

    const Packet serialize();
    void deserialize(const std::vector<uint8_t>& p);

    // float to_float(const std::vector<uint8_t>& p, int i);

    float ax,ay,az; // 4*3 = 12
    float gx,gy,gz; // 12
    float mx,my,mz; // 12
    float temp;     // 4
                    // params = 40
                    // total = 45
};

class PT: public SerialLib {
public:
    PT();

    const Packet serialize();
    void deserialize(const std::vector<uint8_t>& p);

    float pressure, temperature;
};
