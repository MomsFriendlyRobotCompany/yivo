/*
 Do not edit, this is autogenerated
 MIT Kevin Walchko (c) 2023
*/
#pragma once

#include <stdint.h>
#include <string>

namespace foobar {


constexpr uint8_t VEC_MSG_SIZE = 12;

constexpr uint8_t VEC_MSG_ID = 1;

struct __attribute__((packed)) vec_t {
  float x; // size: 4
  float y; // size: 4
  float z; // size: 4
  float magnitude() {
  return sqrtf(x*x + y*y + z*z);
  }
  vec_t normalize() {
  float mag = magnitude();
  return vec_t{x/mag,y/mag,z/mag};
  }

};



inline
message_t vec_yivo_pack(const vec_t& msg) {
  message_t msg = yivo.pack(1, (uint8_t*)&msg, 12);
  return msg;
}




} // end namespace
