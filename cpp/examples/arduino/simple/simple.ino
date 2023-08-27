#include <yivo.hpp>


// #include <yivo/yivo.hpp>

// struct Buffer {
//   union {
//     float    f[10];
//     uint32_t l[10];
//     uint8_t  b[10*sizeof(float)];
//   };
//   const uint32_t size = 10;

//   void clear() {
//     memset(b, 0, size);
//   }
// };

struct Sensor {
    // union {
    //     float f[10];
    //     uint8_t b[10*sizeof(float)];
    //     uint32_t l[10];
    // } data;
    Buffer<256> data;
};


Yivo yivo;
Sensor sen;

void setup() {
  Serial.begin(9600);

  sen.data.clear();

  sen.data.f[0] = 1.1; // x
  sen.data.f[1] = 2.2; //y
  sen.data.f[2] = 3.3; // z
  sen.data.f[3] = 4.4; // temp
  sen.data.l[4] = millis(); // ts

  int err = yivo.pack(IMU_AT, sen.data.b, 5*sizeof(float));

  uint8_t *buff = yivo.get_buffer();
  uint16_t size = yivo.get_total_size();
}

void loop() {
  while (Serial.available() > 0) {
    int b = Serial.read();
    // uint8_t msgid = yivo.read_packet();
    if (b == 'g') {
    // if (msgid == IMU_AT) {
      sen.data.l[4] = millis(); // ts
      int err = yivo.pack_n_send(IMU_AT, sen.data.b, 5*sizeof(float));
    }
    else {
      Serial.println("unknown");
    }
  }
}