#include <assert.h>
#include <scruffy/scruffy.h>
#include <stdio.h>
#include <stdlib.h>
#include <yivo/yivo.h>

#define YIVO_OVERHEAD 6
#define BUFFER_SIZE   64
#define MAX_MSG_SIZE  32

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

// make some globals we can reuse
ypars_t *pars = NULL;
ypkt_t *ym1   = NULL;
ypkt_t *ym2   = NULL;

TEST(yivo, yivo_create) {
  EXPECT_NULL(ym1);
  EXPECT_NULL(ym2);
  EXPECT_NULL(pars);

  // setup yivo packets and parser
  ym1  = ypkt_create(10, sizeof(msg_t));
  ym2  = ypkt_create(20, sizeof(msg2_t));
  pars = ypars_create(MAX_MSG_SIZE);

  EXPECT_NOT_NULL(ym1);
  EXPECT_NOT_NULL(ym2);
  EXPECT_NOT_NULL(pars);
}

// This tests the packer correctly packing the
// binary message
TEST(yivo, ypkt_t) {
  msg_t m = {105, 1000};
  // ypkt_t *ym = ypkt_create(110, sizeof(m));
  ypkt_pack(ym1, (uint8_t *)&m, sizeof(m));

  // valid message, verifies checksum
  EXPECT_EQ(ypkt_valid_msg(ym1), 0);
  // good message start characters
  EXPECT_EQ((char)ym1->data[0], '$');
  EXPECT_EQ((char)ym1->data[1], 'K');
  // correct total message size and payload size
  EXPECT_EQ(ym1->size, sizeof(msg_t) + YIVO_OVERHEAD);
  EXPECT_EQ((ym1->data[3] << 8) | ym1->data[2], sizeof(msg_t));
  // correct message ID
  EXPECT_EQ(ym1->data[4], ym1->msg_id);
}

TEST(yivo, ypkt_t_fails) {
  msg_t m = {105, 1000};

  // wrong buffer length: msg_t -> msg2_t
  int err = ypkt_pack(ym1, (uint8_t *)&m, sizeof(msg2_t));
  EXPECT_FALSE(err == 0);

  // bad checksum
  ypkt_pack(ym1, (uint8_t *)&m, sizeof(msg_t));
  ym1->data[YIVO_CS] = 0;
  EXPECT_FALSE(ypkt_valid_msg(ym1) == 0);

  // bad message start characters
  ypkt_pack(ym1, (uint8_t *)&m, sizeof(msg_t));
  ym1->data[YIVO_H0] = 'X';
  EXPECT_FALSE(ypkt_valid_msg(ym1) == 0);

  // bad payload size
  ypkt_pack(ym1, (uint8_t *)&m, sizeof(msg_t));
  ym1->data[YIVO_LN] = 20;
  EXPECT_FALSE(ypkt_valid_msg(ym1) == 0);
}

// This tests the parser finding multiple messages
// in a buffer and extracting them
TEST(yivo, yivoparser_stream) {
  msg_t a = {-10, 300};
  int err = ypkt_pack(ym1, (uint8_t *)&a, sizeof(a));
  EXPECT_EQ(ypkt_valid_msg(ym1), 0);
  EXPECT_EQ(err, 0);

  msg2_t b = {1248, -3.14, 'w'};
  err      = ypkt_pack(ym2, (uint8_t *)&b, sizeof(b));
  EXPECT_EQ(ypkt_valid_msg(ym2), 0);
  EXPECT_EQ(err, 0);

  // fill buffer with different messages
  memcpy(&buffer[2], ym1->data, ym1->size);
  memcpy(&buffer[15], ym2->data, ym2->size);
  memcpy(&buffer[33], ym1->data, ym1->size);
  memcpy(&buffer[46], ym2->data, ym2->size);

  // bad data --------------------
  ypars_stream(pars, 1);
  ypars_stream(pars, '$'); // false start
  ypars_stream(pars, 72);
  ypars_stream(pars, '$'); // false start
  ypars_stream(pars, '$'); // false start
  ypars_stream(pars, 212);

  // find good messages in buffer
  int msgs_found = 0;
  for (int i = 0; i < BUFFER_SIZE; ++i) {
    uint8_t byte = buffer[i];
    // printf("0x%02X\n", b);
    uint8_t msgid = ypars_stream(pars, byte);
    if (msgid == 0) continue;
    else if (msgid == 10) {
      msgs_found += 1;
      msg_t aa;
      err = ypars_get(pars, (uint8_t *)&aa, sizeof(msg_t));
      EXPECT_EQ(err, 0);
      EXPECT_EQ(aa.a, a.a);
      EXPECT_EQ(aa.b, a.b);
    } else if (msgid == 20) {
      msgs_found += 1;
      msg2_t bb;
      err = ypars_get(pars, (uint8_t *)&bb, sizeof(msg2_t));
      EXPECT_EQ(err, 0);
      EXPECT_EQ(bb.i, b.i);
      EXPECT_EQ(bb.c, b.c);
      EXPECT_FLOAT_EQ(bb.f, b.f);
    }
  }
  EXPECT_EQ(msgs_found, 4);
}

TEST(yivo, yivoparser_buffer) {
  ypars_t *pars = ypars_create(MAX_MSG_SIZE);

  msg_t a     = {-10, 300};
  ypkt_t *msg = ypkt_create(10, sizeof(msg_t));
  int err     = ypkt_pack(msg, (uint8_t *)&a, sizeof(a));
  EXPECT_EQ(ypkt_valid_msg(msg), 0);
  EXPECT_EQ(err, 0);

  msg2_t b     = {1248, -3.14, 'w'};
  ypkt_t *msg2 = ypkt_create(20, sizeof(msg2_t));
  err          = ypkt_pack(msg2, (uint8_t *)&b, sizeof(b));
  EXPECT_EQ(ypkt_valid_msg(msg2), 0);
  EXPECT_EQ(err, 0);

  // fill buffer with different messages
  memcpy(&buffer[2], ym1->data, ym1->size);
  memcpy(&buffer[15], ym2->data, ym2->size);
  memcpy(&buffer[33], ym1->data, ym1->size);
  memcpy(&buffer[46], ym2->data, ym2->size);

  // bad data --------------------
  ypars_stream(pars, 1);
  ypars_stream(pars, '$'); // false start
  ypars_stream(pars, 72);
  ypars_stream(pars, '$'); // false start
  ypars_stream(pars, '$'); // false start
  ypars_stream(pars, 212);

  // find good messages in buffer
  int msgs_found = 0;
  for (uint32_t i = 0; i < BUFFER_SIZE; ++i) {
    uint8_t msgid = ypars_buffer(pars, &buffer[i], BUFFER_SIZE - i, &i);
    if (msgid == 0) continue;
    else if (msgid == 10) {
      // printf("found 10\n");
      msgs_found += 1;
      msg_t aa;
      err = ypars_get(pars, (uint8_t *)&aa, sizeof(msg_t));
      EXPECT_EQ(err, 0);
      EXPECT_EQ(aa.a, a.a);
      EXPECT_EQ(aa.b, a.b);
    } else if (msgid == 20) {
      // printf("found 20\n");
      msgs_found += 1;
      msg2_t bb;
      err = ypars_get(pars, (uint8_t *)&bb, sizeof(msg2_t));
      EXPECT_EQ(err, 0);
      EXPECT_EQ(bb.i, b.i);
      EXPECT_EQ(bb.c, b.c);
      EXPECT_FLOAT_EQ(bb.f, b.f);
    }
  }
  EXPECT_EQ(msgs_found, 4);
}

TEST(yivo, yivo_free) {
  // this is the end, so clean up the globals
  ym1 = ypkt_free(ym1);
  EXPECT_EQ(ym1, NULL);

  ym2 = ypkt_free(ym2);
  EXPECT_EQ(ym2, NULL);

  pars = ypars_free(pars);
  EXPECT_EQ(pars, NULL);
}

RUN_ALL();