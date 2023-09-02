#include <iostream>
#include <stdint.h>
#include <strings.h> // memset
#include <yivo.hpp>
#include <array>
#include <string>
#include <vector>

using namespace std;

// some struct I want to pack
struct Msg_t {
  int a,b,c,d;
};


int main() {
  Msg_t msg{63,64,65,66}; // ?@AB

  Yivo yivo;
  YivoPack_t p = yivo.pack(10, reinterpret_cast<uint8_t *>(&msg), sizeof(Msg_t));

  cout << "Msg[" << sizeof(p) << ']' << endl;
  for (const uint8_t& m: p) cout << m << ',';
  cout << endl;

  return 0;
}
