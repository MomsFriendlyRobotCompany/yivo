

// class Buffer {
// protected:
// // public:
//     static std::array<uint8_t,256> buffer;
//     // static array<uint8_t,256> buffer;
//     static int p;
//     // uint8_t compute_checksum(uint16_t len){
//     //     uint32_t checksum = 0;
//     //     for (uint8_t i = 2; i < len; ++i) checksum += buffer[i];
//     //     return (~checksum) & 0xFF;
//     // }
// public:
//     Buffer() {}
//     // uint16_t i16(uint8_t a, uint8_t b) {return (b << 8) + a;}
//     void reset(){p = 0;}
//     int size(){return p;}
//     void push(int a){
//         buffer[p++] = static_cast<uint8_t>(a);
//     }
//     void push(char a){
//         buffer[p++] = static_cast<uint8_t>(a);
//     }
//     void push(uint8_t a){
//         buffer[p++] = a;
//     }
//     void push(uint8_t id, uint16_t a){
//         buffer[p++] = id;
//         buffer[p++] = 0xff & a;
//         buffer[p++] = a >> 8;
//     }
//     void push(uint8_t id, uint16_t a, uint16_t b){
//         buffer[p++] = id;
//         buffer[p++] = 0xff & a;
//         buffer[p++] = a >> 8;
//         buffer[p++] = 0xff & b;
//         buffer[p++] = b >> 8;
//     }
//
//     void print(){
//         cout << "[" << p << "]";
//         for (int i=0; i<p; ++i) cout << int(buffer[i]) << ",";
//         cout << endl;
//     }
// };
// int Buffer::p = 0;
// // uint8_t Buffer::buffer[] = {0};
// // array<uint8_t,256> Buffer::buffer {0};
//
// class Test: public Buffer {
// public:
//     Test(){
//         reset();
//         push(HEADER);
//         push('s');
//         push(2);
//         push(1,123);
//         push(2,456);
//     }
//
// };
