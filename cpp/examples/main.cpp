#include <iostream>
#include <array>
#include <vector>
#include <stdint.h>
#include <strings.h> // memset
#include <exception>
#include <yivo/yivo.hpp>


using namespace std;

// std::ostream &operator<<(std::ostream &os, Packet const &p) {
//     // for (uint8_t const& u: p.pkt) os << std::hex << int(u) << ",";
//     for (uint8_t const& u: p.pkt) os << int(u) << ",";
//     os << " ";
//     return os;
// }

struct Sensor {
    union {
        float f[10];
        uint8_t b[10*sizeof(float)];
    } data;
};

int main(){
    Yivo yivo;
    Sensor sen;
    sen.data.f[0] = 1.1;
    sen.data.f[1] = 2.2;
    sen.data.f[2] = 3.3;

    int err = yivo.pack(IMU_AT, sen.data.b, 3*sizeof(float));
    cout << err << endl;

    uint8_t *buff = yivo.get_buffer();
    uint16_t size = yivo.get_total_size();

    for (int i=0; i < size; ++i) {
        cout << int(buff[i]) << ",";
    }
    cout << endl;

    ImuA_t imu;
    // memcpy(&imu, &yivo.get_buffer()[5], yivo.get_payload_size());
    // cout << imu.ay << " " << imu.az << endl;

    // msg_t m = yivo.read_packet();
    int id = yivo.read_packet();
    if (id == 1)
        // imu = yivo.unpack<ImuA_t>(yivo.get_payload_buffer(), yivo.get_payload_size());
        imu = yivo.unpack<ImuA_t>();
    else if (false) {}

    cout << imu.ay << " " << imu.az << endl;

    return 0;
}
