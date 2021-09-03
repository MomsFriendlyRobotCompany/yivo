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
#include <yivo/yivo.h>

uint32_t SerialLib::to_uint32(const std::vector<uint8_t>& p, int i){
    uint32_t temp = 0;
    temp = ((p[i]   << 24) |
            (p[i+1] << 16) |
            (p[i+2] <<  8) |
             p[i+3]);
    return temp;
}

int32_t SerialLib::to_int32(const std::vector<uint8_t>& p, int i){
    uint32_t temp = 0;
    temp = ((p[i]   << 24) |
            (p[i+1] << 16) |
            (p[i+2] <<  8) |
             p[i+3]);
    // return temp;
    return *reinterpret_cast<int32_t*>(&temp);
}

uint16_t SerialLib::to_uint16(const std::vector<uint8_t>& p, int i){
    uint16_t temp = ((p[i] << 8) | p[i+1]);
    // return *((uint16_t*) &temp);
    return temp;
}

int16_t SerialLib::to_int16(const std::vector<uint8_t>& p, int i){
    uint16_t temp = ((p[i] << 8) | p[i+1]);
    return *((int16_t*) &temp);
}

float SerialLib::to_float(const std::vector<uint8_t>& p, int i){
    // uint32_t temp = 0;
    // temp = ((p[i]   << 24) |
    //         (p[i+1] << 16) |
    //         (p[i+2] <<  8) |
    //          p[i+3]);
    uint32_t temp = to_uint32(p,i);
    return *((float*) &temp);
}
