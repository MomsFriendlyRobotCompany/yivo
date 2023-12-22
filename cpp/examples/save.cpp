#include <iostream>
#include <stdint.h>
#include <strings.h> // memset
#include <yivo.hpp>
#include <array>
#include <string>
#include <cstdint>
#include <vector>

using namespace std;
using namespace yivo;

// some struct I want to pack
struct Msg_t {
  int a,b;
  uint64_t c,d;
};


int main() {
  Msg_t msg{63,64,65,66}; // ?@AB

  // Parser yivo;
  yivopkt_t p;
  p.pack(10, reinterpret_cast<uint8_t *>(&msg), sizeof(Msg_t));

  cout << "Msg[" << sizeof(p) << ']' << ": ";
  cout << to_string(p) << endl;

  return 0;
}
