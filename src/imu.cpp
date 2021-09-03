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
// #include <yivo/yivo.h>
// #include <exception>
// #include <stdexcept>
//
//
// IMU::IMU() {}
//
// void IMU::accels(const float& x, const float& y, const float& z){
//     ax = x;
//     ay = y;
//     az = z;
// }
//
// void IMU::gyros(const float& x, const float& y, const float& z){
//     gx = x;
//     gy = y;
//     gz = z;
// }
//
// void IMU::mags(const float& x, const float& y, const float& z){
//     mx = x;
//     my = y;
//     mz = z;
// }
//
// const Packet IMU::serialize(){
//     Packet pkt(IMU_MSG,10*4);
//     // 0xff
//     // 0xff
//     // id
//     // len
//     pkt.push(ax);   // 4
//     pkt.push(ay);   // 8
//     pkt.push(az);   // 12
//     pkt.push(gx);   // 16
//     pkt.push(gy);   // 20
//     pkt.push(gz);   // 24
//     pkt.push(mx);   // 28
//     pkt.push(my);   // 32
//     pkt.push(mz);   // 36
//     pkt.push(temp); // 40
//     pkt.end();      // 44
//
//     return pkt;
// }
//
// void IMU::deserialize(const std::vector<uint8_t>& p){
//     if (p[0] != 0xff || p[1] != 0xff || p[2] != IMU_MSG) throw std::runtime_error("invalide packet");
//     if (!Packet::valid_checksum(p)) throw std::runtime_error("invalide packet");
//
//     ax = to_float(p, 4);
//     ay = to_float(p, 8);
//     az = to_float(p, 12);
//     gx = to_float(p, 16);
//     gy = to_float(p, 20);
//     gz = to_float(p, 24);
//     mx = to_float(p, 28);
//     my = to_float(p, 32);
//     mz = to_float(p, 36);
//     temp = to_float(p, 40);
// }
