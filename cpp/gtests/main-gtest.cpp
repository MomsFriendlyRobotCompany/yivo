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

TEST(yivo, yivopkt_t) {
  Yivo yivo(4);
  msg_t m{105, 1000};

  yivopkt_t msg;
  msg.pack(10, reinterpret_cast<uint8_t *>(&m), sizeof(m));

  EXPECT_EQ(msg.size(), sizeof(msg_t)+header_size);
  EXPECT_TRUE(msg.has_valid_checksum());
  EXPECT_TRUE(msg.valid_msg());
  EXPECT_EQ(msg[0], '$');
  EXPECT_EQ(msg[1], 'K');
  EXPECT_EQ(msg.msg_id(), 10);
  EXPECT_EQ(msg.payload_size(), 5);
  EXPECT_EQ(msg.checksum(), msg[10]);
  EXPECT_EQ(msg.size(), 11);
}

TEST(yivo, pack_unpack) {
  msg_t m{105, 1000};

  yivopkt_t msg;
  msg.pack(10, reinterpret_cast<uint8_t *>(&m), sizeof(m));

  msg_t m2 = msg.unpack<msg_t>();
  EXPECT_EQ(m.a, m2.a);
  EXPECT_EQ(m.b, m2.b);
}

TEST(yivo, read) {
  Yivo yivo;
  msg_t m{40, 3100};

  yivopkt_t msg;
  msg.pack(20, reinterpret_cast<uint8_t *>(&m), sizeof(msg_t));
  EXPECT_TRUE(msg.size() == sizeof(msg_t)+header_size);
  // printf("msg id %d\n", (int)msg.msg_id());

  uint8_t id = 0;
  for (uint8_t i=0; i<20; ++i) id = yivo.parse(i);
  EXPECT_EQ(id, 0);

  for (const uint8_t& c: msg) {
    // printf("c %d\n", (int)c);
    id = yivo.parse(c);
  }
  // printf("id %d\n", (int)id);
  EXPECT_EQ(id, 20);

  yivopkt_t p;
  yivo.get_packet(p);
  msg_t m2 = p.unpack<msg_t>();
  EXPECT_EQ(m.a, m2.a);
  EXPECT_EQ(m.b, m2.b);
}