#pragma once

#include "Arduino.h"

/*
Packer p;
p.pack(0x13, 1.,2.);
Serial.write(p.buffer, p.size);
*/
class Packer {
public:
//    enum {
//        
//    }
    Packer(Stream *sp): serialport(sp), cnt(0) {}

    void begin(){
        cnt = 0;
        memcpy(&buffer[cnt], header, 2);
        cnt += 3; // [0xff,0xff,msglen]
    }

    void end(){
        memcpy(&buffer[cnt], ender, 2);
        cnt += 2;
        buffer[2] = cnt;
        serialport->write(buffer, cnt);
    }

    void frame(byte id, float a){
        byte const* p = reinterpret_cast<byte const *>(f);
        f[0] = a;
        buffer[cnt] = id;
        memcpy(&buffer[cnt+1], p, 4);
        cnt += 5;
    }

    void frame(byte id, float a, float b){
        byte const* p = reinterpret_cast<byte const *>(f);
        f[0] = a;
        f[1] = b;
        buffer[cnt] = id;
        memcpy(&buffer[cnt+1], p, 8);
        cnt += 9;
    }

    void frame(const byte id, const float a, const float b, const float c){
        byte const* p = reinterpret_cast<byte const *>(f);
        f[0] = a;
        f[1] = b;
        f[2] = c;
        buffer[cnt] = id;
        memcpy(&buffer[cnt+1], p, 12);
        cnt += 13;
    }
    
    void frame(const byte id, const float a, const float b, const float c, const float d){
        byte const* p = reinterpret_cast<byte const *>(f);
        f[0] = a;
        f[1] = b;
        f[2] = c;
        f[3] = d;
        buffer[cnt] = id;
        memcpy(&buffer[cnt+1], p, 16);
        cnt += 17;
    }

protected:
    float f[4];
    byte buffer[256];
    Stream *serialport;
    const uint8_t header[2] = {0xFF,0xFF};
    const uint8_t ender[2] = {0xEE,0xEE};
    uint8_t cnt;
    
};
