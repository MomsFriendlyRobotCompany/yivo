#include <iostream>
#include <array>
#include <vector>
#include <stdint.h>
#include <strings.h> // memset
#include <exception>
#include <yivo/yivo.h>


using namespace std;

/*
[H,ID,LEN,..PARAMS..,CSUM]

ID  Message Format
a   angle        [H,a,ID,degrees]
b   bulk read    [H,b,len,...]
c   count        [H,c,ID,counts]
s   sync cnt     [H,s,len,id,cnt,id,cnt,id,cnt, ....]
S   sync cnt/spd [H,s,len,id,cnt,speed,id,cnt,speed,id,cnt,speed, ....]
                 len: how many id's
                 ex: [H,s,2,1,123,2,456] => 7B
r   read         [H,r,id,register]
t   torque       [H,t,id,value]
*/


std::ostream &operator<<(std::ostream &os, Packet const &p) {
    // for (uint8_t const& u: p.pkt) os << std::hex << int(u) << ",";
    for (uint8_t const& u: p.pkt) os << int(u) << ",";
    os << " ";
    return os;
}

void pprint(const Packet& p){
    cout << "pprint: " << p  << endl;
}

int main(){
    IMU imu;
    imu.accels(1.2,1.2,1.2);
    imu.gyros(-1.2,-1.2,-1.2);
    imu.mags(41.2,41.2,41.2);
    imu.temp = 450.0;
    cout << ">> gx: " << imu.gx << endl;

    Packet pkt = imu.serialize();
    pprint(pkt);

    IMU imu2;
    imu2.deserialize(pkt.pkt);
    cout << ">> gx: " << imu2.mz << endl;

    PT pt;
    pt.pressure = 100;
    pt.temperature = -34;
    pkt = pt.serialize();
    pprint(pkt);

    return 0;
}
