#include <gtest/gtest.h> // googletest header file
#include <iostream>
#include <stdlib.h>
#include <yivo.hpp>

using namespace std;
constexpr size_t header_size = 6;

struct __attribute__((packed)) msg_t {
  uint8_t a;  // 1
  uint32_t b; // 4
}; // 5 bytes

TEST(yivo, YivoPack_t) {
  Yivo yivo(4);
  yivo.set_header('x','x');
  msg_t m{105, 1000};

  YivoPack_t msg = yivo.pack(10, reinterpret_cast<uint8_t *>(&m), sizeof(m));

  EXPECT_EQ(msg.size(), sizeof(msg_t)+header_size);
  EXPECT_TRUE(yivo.valid_msg(msg));
  EXPECT_TRUE(yivo.valid_msg(msg.data(), msg.size()));
  EXPECT_EQ(msg[0], 'x');
  EXPECT_EQ(msg[1], 'x');
  EXPECT_EQ(msg.msgid(), 10);
  EXPECT_EQ(msg.playload_size(), 5);
}

TEST(yivo, pack_unpack) {
  Yivo yivo;
  msg_t m{105, 1000};

  YivoPack_t msg = yivo.pack(10, reinterpret_cast<uint8_t *>(&m), sizeof(m));

  msg_t m2 = yivo.unpack<msg_t>(msg);
  EXPECT_EQ(m.a, m2.a);
  EXPECT_EQ(m.b, m2.b);

  msg_t m3 = yivo.unpack<msg_t>(msg.data(), msg.size());
  EXPECT_EQ(m.a, m3.a);
  EXPECT_EQ(m.b, m3.b);
}

TEST(yivo, read) {
  Yivo yivo;
  msg_t m{40, 3100};

  YivoPack_t msg = yivo.pack(20, reinterpret_cast<uint8_t *>(&m), sizeof(msg_t));
  EXPECT_TRUE(msg.size() == sizeof(msg_t)+header_size);

  uint8_t id = YIVO_NO_ID;
  for (const uint8_t& c: msg) id = yivo.read(c);
  EXPECT_EQ(id, 20);

  msg_t m2 = yivo.unpack<msg_t>();
  EXPECT_EQ(m.a, m2.a);
  EXPECT_EQ(m.b, m2.b);
}