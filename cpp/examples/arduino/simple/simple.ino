#include <yivo.hpp>

constexpr uint8_t MSG_ID = 10;

struct Sensor {
  float a,b,c;
  long d;
};


Yivo yivo;
Sensor sen;
YivoPack_t ret;

void setup() {
  Serial.begin(9600);
}

void loop() {
  // see if we received a message?
  uint8_t id = 0;
  int num = Serial.available();
  while (num-- > 0) {
    int b = Serial.read();
    id = yivo.read(b);
    if (id > 0) break;
  }

  Sensor s;
  if (id == MSG_ID) s = yivo.unpack<Sensor>(); // decode the message

  // send a message back
  sen.a = 1.1; // x
  sen.b = 2.2; // y
  sen.c = 3.3; // z
  sen.d = millis(); // timestamp

  ret = yivo.pack(
    MSG_ID,
    reinterpret_cast<uint8_t *>(&sen),
    sizeof(Sensor));

  Serial.write(ret.data(), ret.size());
}