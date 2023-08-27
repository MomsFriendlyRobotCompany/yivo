#include <iostream>
#include <stdint.h>
#include <strings.h> // memset
#include <yivo.hpp>
#include <array>
#include <string>
#include <vector>

using namespace std;

// some struct I want to pack
struct Sensor {
  float f[3];
};

// struct message_t {
//   message_t(uint8_t* buff, size_t size) {

//   }
// }

// typedef string message_t;
// typedef vector<uint8_t> message_t;
class message_t: public vector<uint8_t> {
  public:
  message_t() {}
  void set(uint8_t* buff, size_t s) {
    this->assign(buff, buff + s);
  }
  // template<typename T>
  // T o
};

int main() {
  uint8_t b[] = {61,62,63,64,65,66}; // =>?@AB
  message_t msg;
  // message_t msg(reinterpret_cast<const char*>(b),6);
  // msg.append(reinterpret_cast<const char*>(b),6);
  // msg.assign(6,b);
  // msg.assign(b,b+6);
  msg.set(b,6);

  // cout << msg << endl;
  for (const uint8_t& m: msg) cout << (int)m << ',';
  cout << endl;

  // Yivo yivo;
  // Sensor sen;
  // sen.f[0] = 1.1;
  // sen.f[1] = 2.2;
  // sen.f[2] = 3.3;

  // yivo.pack(10, reinterpret_cast<uint8_t *>(&sen), sizeof(Sensor));

  // uint8_t *buff = yivo.get_buffer();
  // uint16_t size = yivo.get_total_size();

  // Sensor s2     = yivo.unpack<Sensor>();
  // if (sen.f[0] == s2.f[0] && sen.f[1] == s2.f[1] && sen.f[2] == s2.f[2]) {
  //   cout << "good!" << endl;
  // }
  // else cout << "crap!" << endl;

  return 0;
}
