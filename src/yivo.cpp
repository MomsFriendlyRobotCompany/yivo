#include <yivo/yivo.h>

Packet::Packet(uint8_t id, uint8_t len){
    pkt.reserve(len+5);
    // 0xff,0xff,id,len
    pkt.push_back(0xff);
    pkt.push_back(0xff);
    pkt.push_back(id);
    pkt.push_back(len+1); // params + chksum
}

int Packet::size(){return pkt.size();}

void Packet::push(const int& a){
    pkt.push_back(static_cast<uint8_t>(a));
}

void Packet::push(const char& a){
    pkt.push_back(static_cast<uint8_t>(a));
}

void Packet::push(const uint8_t& a){
    pkt.push_back(a);
}
void Packet::push(uint16_t a){
    uint8_t *c = reinterpret_cast<uint8_t*>(&a);
    pkt.push_back(c[1]); // MSB
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

// float Packet::to_float(int i){
//     uint32_t temp = 0;
//     temp = ((p[i]   << 24) |
//             (p[i+1] << 16) |
//             (p[i+2] <<  8) |
//              p[i+3]);
//     return *((float*) &temp);
// }

uint8_t Packet::compute_checksum(){
    uint32_t checksum = 0;
    for (uint8_t i = 2; i < pkt.size(); ++i) checksum += pkt[i];
    return (~checksum) & 0xFF;
}

///////////////////////////////////////////////////////////////////////////////

float SerialLib::to_float(const std::vector<uint8_t>& p, int i){
    uint32_t temp = 0;
    temp = ((p[i]   << 24) |
            (p[i+1] << 16) |
            (p[i+2] <<  8) |
             p[i+3]);
    return *((float*) &temp);
}


IMU::IMU() {}

void IMU::accels(const float& x, const float& y, const float& z){
    ax = x;
    ay = y;
    az = z;
}

void IMU::gyros(const float& x, const float& y, const float& z){
    gx = x;
    gy = y;
    gz = z;
}

void IMU::mags(const float& x, const float& y, const float& z){
    mx = x;
    my = y;
    mz = z;
}

const Packet IMU::serialize(){
    Packet pkt(IMU_MSG,10*4);
    // 0xff
    // 0xff
    // id
    // len
    pkt.push(ax);   // 4
    pkt.push(ay);   // 8
    pkt.push(az);   // 12
    pkt.push(gx);   // 16
    pkt.push(gy);   // 20
    pkt.push(gz);   // 24
    pkt.push(mx);   // 28
    pkt.push(my);   // 32
    pkt.push(mz);   // 36
    pkt.push(temp); // 40
    pkt.end();      // 44

    return pkt;
}

// float IMU::to_float(const std::vector<uint8_t>& p, int i){
//     uint32_t temp = 0;
//     temp = ((p[i]   << 24) |
//             (p[i+1] << 16) |
//             (p[i+2] <<  8) |
//              p[i+3]);
//     return *((float*) &temp);
// }

void IMU::deserialize(const std::vector<uint8_t>& p){
    if (p[0] != 0xff || p[1] != 0xff || p[2] != IMU_MSG) throw std::runtime_error("invalide packet");
    if (!Packet::valid_checksum(p)) throw std::runtime_error("invalide packet");

    ax = to_float(p, 4);
    ay = to_float(p, 8);
    az = to_float(p, 12);
    gx = to_float(p, 16);
    gy = to_float(p, 20);
    gz = to_float(p, 24);
    mx = to_float(p, 28);
    my = to_float(p, 32);
    mz = to_float(p, 36);
    temp = to_float(p, 40);
}

/////////////////////////////////////////////////////////////////////////////

PT::PT(){}

const Packet PT::serialize(){

    Packet pkt(PT_MSG,2*4);
    // 0xff
    // 0xff
    // id
    // len
    pkt.push(pressure);    // 4
    pkt.push(temperature); // 8
    pkt.end();

    return pkt;
}
void PT::deserialize(const std::vector<uint8_t>& p){
    if (p[0] != 0xff || p[1] != 0xff || p[2] != PT_MSG) throw std::runtime_error("invalide packet");
    if (!Packet::valid_checksum(p)) throw std::runtime_error("invalide packet");

    pressure = to_float(p, 4);
    temperature = to_float(p, 8);
}
