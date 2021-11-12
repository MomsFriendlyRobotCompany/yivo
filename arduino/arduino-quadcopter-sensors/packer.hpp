#pragma once

/*
Packer p;
p.pack(0x13, 1.,2.);
Serial.write(p.buffer, p.size);
*/
class Packer {
public:
    enum {
        
    }
    Packer() {}

    void frame(byte id, float a){
        byte const* p = reinterpret_cast<byte const *>(f);
        f[0] = a;
        bf[0] = id;
        memcpy(&bf[1], p, 4);
        // Serial.write(bf, 5);
        size = 5;
    }

    void frame(byte id, float a, float b){
        byte const* p = reinterpret_cast<byte const *>(f);
        f[0] = a;
        f[1] = b;
        bf[0] = id;
        memcpy(&bf[1], p, 8);
        // Serial.write(bf, 9);
        size = 9;
    }

    void frame(const byte id, const float a, const float b, const float c){
        byte const* p = reinterpret_cast<byte const *>(f);
        f[0] = a;
        f[1] = b;
        f[2] = c;
        bf[0] = id;
        memcpy(&bf[1], p, 12);
        // Serial.write(bf, 13);
        size = 13;
    }

    uint8_t size;
    byte bf[13]; // rename buffer

protected:
    float f[4];
}
