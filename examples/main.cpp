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

/*
b sint8  [1]
B uint8  [1]
h sint16 [2]
H uint16 [2]
i sint32 [4]
I uint32 [4]
f float  [4]
d double [8]
*/

typedef struct {
    int id;
    std::vector<uint8_t> data;
    // std::array<uint8_t> data;
} Raw;

bool find_packet(const std::vector<uint8_t>& buf, std::vector<Raw>& vr){
    bool h1 = false;
    bool h2 = false;
    for (int i=0; i < buf.size(); ++i){
        uint8_t b = buf[i];
        cout << i << endl;
        if (h1 && h2){
            Raw r;
            r.id = b; // i => id
            int len = buf[i+1]; // i+1 => len
            r.data.reserve(len);
            uint32_t chksum = 0;
            // int ii = 0;
            for (int j = i+2; j<i+len+1; ++j){
                uint8_t d = buf[j];
                chksum += d;
                r.data.push_back(d);
            }
            uint8_t cs = buf[i+len+1]; // get packet checksum
            i += len + 1; // set i to end of packet
            cout << "cs: " << int(cs) <<"   " << int((~chksum) & 0xFF) << endl;
            if((~chksum & 0xFF) == cs) vr.push_back(r); // save if good packet
            h1 = false; // reset header
            h2 = false; // reset header
        }
        else if (b == 0xff && !h1) h1 = true;
        else if (b == 0xff && h1) h2 = true;
    }
    bool ret = (vr.size() > 0) ? true : false;
    return ret;
}



// int32_t to_int32()

int main(){
    SerialLib s;
    Packet pkt(22, "fff");
    pkt.push(10.0f);
    pkt.push(-3.14567890f);
    pkt.push(100000.0f);
    pkt.end();
    cout << ">> " << pkt << endl;

    Packet p2(33, "bIH");
    p2.push((int8_t)-125);
    p2.push((uint32_t)3000);
    p2.push((uint16_t)1600);
    p2.end();
    cout << ">> " << p2 << endl;

    vector<uint8_t> buffer;
    for (auto const& b: pkt.pkt) buffer.push_back(b);
    for (auto const& b: p2.pkt) buffer.push_back(b);

    vector<Raw> vr;
    cout << "=========================================" << endl;
    bool ans = find_packet(buffer, vr);
    if (ans) {
        for (auto const& r: vr){
            cout << r.id << " [" << r.data.size() << "]" << endl;
            for (auto const& b: r.data) cout << int(b) << ",";
            cout << endl;

            if (r.id == 33){
                cout << "ID: " << r.id << endl;
                cout << "|-> " << int(int8_t(r.data[0])) << endl;
                cout << "|-> " << s.to_uint32(r.data, 1) << endl;
                cout << "|-> " << s.to_uint16(r.data, 5) << endl;
            }
        }
    }

    return 0;
}





// int main(){
//     IMU imu;
//     imu.accels(1.2,1.2,1.2);
//     imu.gyros(-1.2,-1.2,-1.2);
//     imu.mags(41.2,41.2,41.2);
//     imu.temp = 450.0;
//     cout << ">> gx: " << imu.gx << endl;
//
//     Packet pkt = imu.serialize();
//     pprint(pkt);
//
//     IMU imu2;
//     imu2.deserialize(pkt.pkt);
//     cout << ">> gx: " << imu2.mz << endl;
//
//     PT pt;
//     pt.pressure = 100;
//     pt.temperature = -34;
//     pkt = pt.serialize();
//     pprint(pkt);
//
//     return 0;
// }
