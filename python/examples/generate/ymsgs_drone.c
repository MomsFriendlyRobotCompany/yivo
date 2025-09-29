
#include <string.h> // memset, memcpy
#include "ymsgs_drone.h"



void vec2f_init(vec2f_t *msg) {
  memset(msg, 0, DRONE_VEC2F_SIZE);
  msg->x = 0; 
  msg->y = 0; 
  
}

void vec3f_init(vec3f_t *msg) {
  memset(msg, 0, DRONE_VEC3F_SIZE);
  msg->x = 0.0; 
  msg->y = 0.0; 
  msg->z = 0.0; 
  
}

void quaternion_init(quaternion_t *msg) {
  memset(msg, 0, DRONE_QUATERNION_SIZE);
  msg->w = 1; 
  msg->x = 0; 
  msg->y = 0; 
  msg->z = 0; 
  
}

void header_init(header *msg) {
  memset(msg, 0, DRONE_HEADER_SIZE);
  
}

void imuf_init(imuf_t *msg) {
  memset(msg, 0, DRONE_IMUF_SIZE);
  
}

void press_init(press_t *msg) {
  memset(msg, 0, DRONE_PRESS_SIZE);
  
}

void pose_init(pose_t *msg) {
  memset(msg, 0, DRONE_POSE_SIZE);
  
}

void calibrate_init(calibrate_t *msg) {
  memset(msg, 0, DRONE_CALIBRATE_SIZE);
  
}

void cal_params_init(cal_params_t *msg) {
  memset(msg, 0, DRONE_CAL_PARAMS_SIZE);
  
}


