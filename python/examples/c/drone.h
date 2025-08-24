#pragma once

#include <stdint.h>
#include <stdbool.h>


#ifdef __cplusplus
extern "C" {
#endif

// message sizes ------------------------------------------
#define DRONE_VEC2F_SIZE 8
#define DRONE_VEC3F_SIZE 12
#define DRONE_QUATERNION_SIZE 16
#define DRONE_HEADER_SIZE 12
#define DRONE_IMUF_SIZE 40
#define DRONE_PRESS_SIZE 8
#define DRONE_POSE_SIZE 40
#define DRONE_CALIBRATE_SIZE 4
#define DRONE_CAL_PARAMS_SIZE 52

// message ids --------------------------------------------
#define DRONE_IMUF_ID 100
#define DRONE_PRESS_ID 101
#define DRONE_POSE_ID 102
#define DRONE_CALIBRATE_ID 103
#define DRONE_CAL_PARAMS_ID 104

// message names ------------------------------------------
#define DRONE_MSG_100 "imuf_t"
#define DRONE_MSG_101 "press_t"
#define DRONE_MSG_102 "pose_t"
#define DRONE_MSG_103 "calibrate_t"
#define DRONE_MSG_104 "cal_params_t"



// Power supply technology (chemistry) constants
// add some other comments
// and this too
typedef enum {
  BATTERY_TECH_E_UNKNOWN = 0,  // Unknown battery technology 
  BATTERY_TECH_E_NIMH = -1,  // Nickel-Metal Hydride battery 
  BATTERY_TECH_E_LION = -2,  // Lithium-ion battery 
  BATTERY_TECH_E_LIPO = 3,  // Lithium Polymer battery 
  BATTERY_TECH_E_LIFE = 4,  // Lithium Iron Phosphate battery 
  BATTERY_TECH_E_NICD = 5,  // Nickel-Cadmium battery 
  BATTERY_TECH_E_LIMN = 6,  // Lithium Manganese Dioxide battery 
  BATTERY_TECH_E_TERNARY = 7,  // Ternary Lithium battery 
  BATTERY_TECH_E_VRLA = 8 // Valve Regulated Lead-Acid battery 
} battery_tech_e;

typedef enum {
  FRAMES_E_ROBOT = 1, 
  FRAMES_E_IMU = 2, 
  FRAMES_E_CAMERA = 3
} frames_e;

typedef enum {
  CALIBRATION_E_ACCEL = 1, 
  CALIBRATION_E_GYRO = 2, 
  CALIBRATION_E_MAG = 4, 
  CALIBRATION_E_MOTORS = 8
} calibration_e;




typedef struct __attribute__((packed)) {
  float x; 
  float y; 
} vec2f_t;

typedef struct __attribute__((packed)) {
  float x; 
  float y; 
  float z; 
} vec3f_t;

typedef struct __attribute__((packed)) {
  float w; 
  float x; 
  float y; 
  float z; 
} quaternion_t;

typedef struct __attribute__((packed)) {
  uint64_t timestamp; 
  frames_e frame; 
} header;

typedef struct __attribute__((packed)) {
  vec3f_t a; 
  vec3f_t g; 
  vec3f_t m; 
  float temperature; 
} imuf_t;

typedef struct __attribute__((packed)) {
  float pressure; 
  float temperature; 
} press_t;

typedef struct __attribute__((packed)) {
  vec3f_t pos; 
  vec3f_t vel; 
  quaternion_t q; 
} pose_t;

// This is a calibration command
typedef struct __attribute__((packed)) {
  calibration_e cal; 
} calibrate_t;

// This is the result of a calibration command
typedef struct __attribute__((packed)) {
  float cal[12];  // cal matrix Ax+B 
  calibration_e cal;  // which sensor 
} cal_params_t;


#ifdef __cplusplus
}
#endif

