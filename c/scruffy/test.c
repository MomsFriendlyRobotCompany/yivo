#include <assert.h>
#include <scruffy/scruffy.h>
#include <stdio.h>
#include <stdlib.h>
#include <yivo/yivo.h>

#define YIVO_OVERHEAD 6
#define BUFFER_SIZE   64

#define YIVO_H0 0
#define YIVO_H1 1
#define YIVO_LN 2
#define YIVO_HN 3
#define YIVO_ID 4
#define YIVO_CS 5
#define YIVO_PL 6

uint8_t buffer[BUFFER_SIZE];

typedef struct YMSG {
  int8_t a;   // 1
  uint32_t b; // 4
} msg_t;      // 6+1+4 = 11

typedef struct YMSG {
  int i;   // 4
  float f; // 4
  char c;  // 1
} msg2_t;  // 6+4+4+1 = 15

// This tests the packer correctly packing the
// binary message
TEST(yivo, yivopkt_t) {
  msg_t m       = {105, 1000};
  yivopkt_t *ym = ypkt_create(sizeof(m));
  ypkt_pack(ym, 110, (uint8_t *)&m, sizeof(m));

  // valid message, verifies checksum
  EXPECT_EQ(ypkt_valid_msg(ym), 0);
  // good message start characters
  EXPECT_EQ((char)ym->data[0], '$');
  EXPECT_EQ((char)ym->data[1], 'K');
  // correct total message size and payload size
  EXPECT_EQ(ym->size, sizeof(msg_t) + YIVO_OVERHEAD);
  EXPECT_EQ((ym->data[3] << 8) | ym->data[2], sizeof(msg_t));
  // correct message ID
  EXPECT_EQ(ym->data[4], 110);
}

TEST(yivo, yivopkt_t_fails) {
  msg_t m       = {105, 1000};
  yivopkt_t *ym = ypkt_create(sizeof(m));

  // wrong buffer length: msg_t -> msg2_t
  int err = ypkt_pack(ym, 110, (uint8_t *)&m, sizeof(msg2_t));
  EXPECT_FALSE(err == 0);

  // bad checksum
  ypkt_pack(ym, 110, (uint8_t *)&m, sizeof(msg_t));
  ym->data[YIVO_CS] = 0;
  EXPECT_FALSE(ypkt_valid_msg(ym) == 0);

  // bad message start characters
  ypkt_pack(ym, 110, (uint8_t *)&m, sizeof(msg_t));
  ym->data[YIVO_H0] = 'X';
  EXPECT_FALSE(ypkt_valid_msg(ym) == 0);

  // bad payload size
  ypkt_pack(ym, 110, (uint8_t *)&m, sizeof(msg_t));
  ym->data[YIVO_LN] = 20;
  EXPECT_FALSE(ypkt_valid_msg(ym) == 0);
}

// This tests the parser finding multiple messages
// in a buffer and extracting them
TEST(yivo, yivo_parser_t) {
  yivo_parser_t *pars = yivo_parse_create();

  msg_t a        = {-10, 300};
  yivopkt_t *msg = ypkt_create(sizeof(msg_t));
  int err        = ypkt_pack(msg, 10, (uint8_t *)&a, sizeof(a));
  EXPECT_EQ(ypkt_valid_msg(msg), 0);
  EXPECT_EQ(err, 0);

  msg2_t b        = {1248, -3.14, 'w'};
  yivopkt_t *msg2 = ypkt_create(sizeof(msg2_t));
  err             = ypkt_pack(msg2, 20, (uint8_t *)&b, sizeof(b));
  EXPECT_EQ(ypkt_valid_msg(msg2), 0);
  EXPECT_EQ(err, 0);

  // fill buffer with different messages
  memcpy(&buffer[2], msg->data, msg->size);
  memcpy(&buffer[15], msg2->data, msg2->size);
  memcpy(&buffer[33], msg->data, msg->size);
  memcpy(&buffer[46], msg2->data, msg2->size);

  // bad data --------------------
  yivo_parse(pars, 1);
  yivo_parse(pars, '$'); // false start
  yivo_parse(pars, 72);
  yivo_parse(pars, '$'); // false start
  yivo_parse(pars, '$'); // false start
  yivo_parse(pars, 212);

  // find good messages in buffer
  int msgs_found = 0;
  for (int i = 0; i < BUFFER_SIZE; ++i) {
    uint8_t byte = buffer[i];
    // printf("0x%02X\n", b);
    uint8_t msgid = yivo_parse(pars, byte);
    if (msgid == 0) continue;
    else if (msgid == 10) {
      msgs_found += 1;
      msg_t aa;
      err = yivo_parse_get(pars, (uint8_t *)&aa, sizeof(msg_t));
      EXPECT_EQ(err, 0);
      EXPECT_EQ(aa.a, a.a);
      EXPECT_EQ(aa.b, a.b);
    } else if (msgid == 20) {
      msgs_found += 1;
      msg2_t bb;
      err = yivo_parse_get(pars, (uint8_t *)&bb, sizeof(msg2_t));
      EXPECT_EQ(err, 0);
      EXPECT_EQ(bb.i, b.i);
      EXPECT_EQ(bb.c, b.c);
      EXPECT_FLOAT_EQ(bb.f, b.f);
    }
  }
  EXPECT_EQ(msgs_found, 4);
}

RUN_ALL();