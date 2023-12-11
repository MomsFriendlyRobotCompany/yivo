/*
 Do not edit, this is autogenerated
 MIT Kevin Walchko (c) 2023
*/
#pragma once

#include <stdint.h>
#include <string>

namespace foobar {


constexpr uint8_t QUAT_MSG_SIZE = 16;

constexpr uint8_t QUAT_MSG_ID = 2;

struct __attribute__((packed)) quat_t {
  float w; // size: 4
  float x; // size: 4
  float y; // size: 4
  float z; // size: 4
  float magnitude() {
  return sqrtf(w*w + x*x + y*y + z*z);
  }
  vec_t normalize() {
  float mag = magnitude();
  return quat_t{w/mag,x/mag,y/mag,z/mag};
  }

};



inline
message_t quat_yivo_pack(const quat_t& msg) {
  message_t msg = yivo.pack(2, (uint8_t*)&msg, 16);
  return msg;
}




} // end namespace
