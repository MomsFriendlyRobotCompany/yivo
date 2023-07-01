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
  sen.f[0] = 1.1;
  sen.f[1] = 2.2;
  sen.f[2] = 3.3;

  yivo.pack(10, reinterpret_cast<uint8_t *>(&sen), sizeof(Sensor));

  uint8_t *buff = yivo.get_buffer();
  uint16_t size = yivo.get_total_size();

  Sensor s2     = yivo.unpack<Sensor>();
  if (sen.f[0] == s2.f[0] && sen.f[1] == s2.f[1] && sen.f[2] == s2.f[2]) {
    cout << "good!" << endl;
  }
  else cout << "crap!" << endl;

  return 0;
}
