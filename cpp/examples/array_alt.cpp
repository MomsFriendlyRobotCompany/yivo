#include <stdio.h>
#include <stdint.h>
// #include <array>
#include <vector>
// #include <iterator>

using namespace std;

// typedef uint8_t* msg_t;


class yivopkt_t {
  public:
  yivopkt_t() {}

  yivopkt_t(const yivopkt_t&) = delete;
  yivopkt_t(const yivopkt_t&&) = delete;
  yivopkt_t& operator=(const yivopkt_t&) = delete;

  ~yivopkt_t() { if (buffer != nullptr) delete[] buffer; printf("del\n"); }

  void pack(uint8_t msgid, uint8_t* data, uint16_t len) {
    if (data == nullptr) return;
    resize(len+6);
    buffer[0] = '$';
    buffer[1] = 'K';
    buffer[2] = (len & 0x00FF); // lo
    buffer[3] = (len >> 8); // hi
    buffer[4] = msgid;
    memcpy(&buffer[5], data, len);
    buffer[len+5] = calc_checksum(); // fixme
  }

  inline uint16_t size(){ return buffer_size; }
  inline const uint8_t* data() const {return buffer;}
  inline const uint8_t operator[](uint16_t i) const {return buffer[i];}
  inline uint8_t msg_id() { return buffer[4]; }
  inline uint16_t payload_size() { return ((uint16_t)buffer[3] << 8) | buffer[2]; }
  inline uint8_t checksum() { return buffer[buffer_size - 1]; }
  inline bool has_valid_chksum() { return calc_checksum() == checksum(); }

  bool valid_msg() {
    if (buffer == nullptr) return false;
    if (buffer_size < 6) return false;
    if ((buffer[0] != '$') || (buffer[1] != 'K')) return false;
    if ((buffer_size - 6) != ((buffer[3] << 8) | buffer[2])) return false;
    if (buffer[4] == 0) return false;
    return has_valid_chksum();
  }

  const uint8_t* begin() const { return buffer; }
  const uint8_t* end() const { return buffer+buffer_size; }

  protected:
  void resize(uint16_t s) {
    if (buffer != nullptr) delete [] buffer;
    printf("new %d\n", (int)s);
    buffer_size = s;
    buffer = new uint8_t[buffer_size];
  }

  uint8_t calc_checksum() {
    uint8_t cs = 0;
    uint16_t pl_size = payload_size();
    for (uint16_t i = 0; i < pl_size; ++i) {
      cs ^= buffer[2+i];
    }
    return cs;
  }

  uint8_t* buffer=nullptr;
  uint16_t buffer_size{0};

};


void p2(const yivopkt_t& m) {
  // for (size_t i=0; i<5; ++i) printf("%u\n", m[i]);

  printf("p2: ");
  for (const uint8_t& b: m) {
    printf("%d,", (int)b);
  }
  printf("\n");
}

#define DATA_SIZE 32

int main() {
  uint8_t data[DATA_SIZE];
  for (int i=0; i<DATA_SIZE; i++) data[i] = i;
  yivopkt_t m;
  m.pack(10, data, sizeof(data));
  p2(m);

  uint8_t *ptr = (uint8_t*) m.data();
  printf("%c,%c,", ptr[0],ptr[1]);
  for (int i=0; i<DATA_SIZE+6-2; i++) printf("%d,", (int)ptr[i+2]);
  printf("\n");

  printf("vector test:\n");

  vector<uint8_t> v;
  v.push_back('x');
  v.push_back('x');
  v.push_back(5);
  v.push_back(0);
  v.push_back(10);
  for (size_t i=0; i<DATA_SIZE; ++i) v.push_back(data[i]);
  v.push_back(222);

  printf("m_t %lu %d  vect %lu %lu  data %lu\n", sizeof(m.data()), m.size(), v.size(), v.capacity(), sizeof(data));
  printf("cksum %d\n", m.checksum());

  return 0;
}