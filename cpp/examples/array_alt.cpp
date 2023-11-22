#include <stdio.h>
#include <stdint.h>
#include <array>

using namespace std;

typedef uint8_t* msg_t;

struct m_t {
  m_t(size_t s): _sz(s) { data = new uint8_t[s]; printf("new\n"); }

  m_t(const m_t&) = delete;
  m_t(const m_t&&) = delete;
  m_t& operator=(const m_t&) = delete;

  ~m_t() { if (data != nullptr) delete[] data; printf("del\n"); }

  uint8_t *data = nullptr;
  uint16_t _sz = 0;

  size_t size() {return _sz;}
  uint8_t msgid() {return data[4];}
  uint16_t payload_size() { return 1; }
};

void p(const m_t& m) {
  m.data[0] = 99;
  for (size_t i=0; i<5; ++i) printf("%u\n", m.data[i]);
}


int main() {
  array<uint8_t, 5> bob{1,2,3,4,5};

  for (size_t i=0; i<5; ++i) printf("%u\n", bob[i]);

  uint8_t buff[10]{0,9,8,7,6,5,4,3,2,1};
  msg_t pkt = buff;

  for (size_t i=0; i<10; ++i) printf("%u\n", pkt[i]);

  m_t m(5);
  // for (size_t i=0; i<5; ++i) printf("%u\n", m.data[i]);
  p(m);

  return 0;
}