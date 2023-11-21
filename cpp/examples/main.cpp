#include <iostream>
#include <stdint.h>
#include <strings.h> // memset
#include <yivo.hpp>

using namespace std;

// some struct I want to pack
struct Sensor {
  float f[3];
};

int main() {
  Yivo yivo;
  Sensor sen;
  sen.f[0] = 1.1f;
  sen.f[1] = 20.02f;
  sen.f[2] = 300.003f;

  YivoPack_t ret = yivo.pack(
    10,
    reinterpret_cast<uint8_t *>(&sen),
    sizeof(Sensor));

  // simulate reading in on byte at a time
  uint8_t id;
  for (const uint8_t& m: ret) {
    id = yivo.parse(m); // fill internal buffer
  }

  // double check we get entire message
  if (id == 0) cout << "FAIL read" << endl;

  // turn bytes into message struct
  Sensor s2 = yivo.unpack<Sensor>();

  if (sen.f[0] == s2.f[0] && sen.f[1] == s2.f[1] && sen.f[2] == s2.f[2]) {
    // cout << "id: " << (int)yivo.get_buffer_msgid() << endl;
    cout << "good!" << endl;
  }
  else cout << "FAIL unpack" << endl;

  return 0;
}
