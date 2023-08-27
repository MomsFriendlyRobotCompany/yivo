#include <gtest/gtest.h> // googletest header file
#include <iostream>
#include <stdlib.h>
#include <yivo.hpp>

using namespace std;

struct __attribute__((packed)) msg_t {
  uint8_t a;  // 1
  uint32_t b; // 4
};

// void equal(msg_t& a, msg_t& b) {
//   EXPECT_TRUE(a.a == b.a);
//   EXPECT_TRUE(a.b == b.b);
// }

TEST(protocol_2, dummy) { EXPECT_TRUE(true); }

TEST(packing, simple) {
  // struct __attribute__ ((packed)) msg_t {
  //   uint8_t a;
  //   uint32_t b;
  // };

  Yivo yivo;
  msg_t m{10, 1000};

  yivo.pack(10, reinterpret_cast<uint8_t *>(&m), sizeof(m));

  uint8_t buf[64];
  memcpy(buf, yivo.get_buffer(), yivo.get_total_size());
  EXPECT_TRUE(yivo.valid_msg(buf));

  uint8_t *pmsg = yivo.get_buffer();
  for (int i = 0; i < yivo.get_total_size(); ++i)
    EXPECT_TRUE(buf[i] == pmsg[i]);

  msg_t m2 = yivo.unpack<msg_t>();
  EXPECT_TRUE(m.a == m2.a);
  EXPECT_TRUE(m.b == m2.b);
  // equal(m,m2);

  msg_t m3 = yivo.unpack_buffer<msg_t>(buf);
  EXPECT_TRUE(m.a == m3.a);
  EXPECT_TRUE(m.b == m3.b);
}

TEST(packing, read) {

  Yivo yivo;
  msg_t m{10, 1000};

  yivo.pack(10, reinterpret_cast<uint8_t *>(&m), sizeof(m));

  // cout << "pack size: " << yivo.get_payload_size() << endl;

  uint8_t buf[64];
  memcpy(buf, yivo.get_buffer(), yivo.get_total_size());
  EXPECT_TRUE(yivo.valid_msg(buf));

  // uint8_t *pmsg = yivo.get_buffer();
  bool ok = false;
  for (int i = 0; i < yivo.get_total_size(); ++i) {
    uint8_t c = buf[i];
    ok        = yivo.read(c);
    // cout << ok << " " << int(c) << " " << char(c) << endl;
  }
  EXPECT_TRUE(ok);
  // cout << "read size: " << yivo.get_payload_size() << endl;

  msg_t m2 = yivo.unpack<msg_t>();
  EXPECT_TRUE(m.a == m2.a);
  EXPECT_TRUE(m.b == m2.b);

  // msg_t m3 = yivo.unpack_buffer<msg_t>(buf);
  // EXPECT_TRUE(m.a == m3.a);
  // EXPECT_TRUE(m.b == m3.b);
}
