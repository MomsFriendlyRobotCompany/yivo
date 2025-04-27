#include "yivo/parser.h"
#include "yivo/yivopkt.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h> // memcpy, memset

typedef enum : uint8_t {
  NONE_STATE, // 0
  H0_STATE,   // 1
  H1_STATE,   // 2
  S0_STATE,   // 3
  S1_STATE,   // 4
  TYPE_STATE, // 5
  DATA_STATE, // 6
  CS_STATE    // 7
} read_state_e;

static void yivo_parser_reset(yivo_parser_t *y) {
  y->state.payload_size = 0;
  y->state.readState    = NONE_STATE;
  y->state.buffer_msgid = 0;
  y->p                  = 0;

  memset(y->buffer, 0, y->buffer_size);
}

bool yivo_parse_init(yivo_parser_t *y, uint32_t size) {
  if (y == NULL) return false;

  y->buffer = (uint8_t *)malloc(size);
  if (y->buffer == NULL) return false;
  y->buffer_size = size;
  yivo_parser_reset(y);

  memset(y->buffer, 0, y->buffer_size);

  return true;
}

//
// Read in one byte at a time, yivo keeps track of the state until it
// finds a good message.
// Returns: bool true - valid message, false - no message yet
uint8_t yivo_parse(yivo_parser_t *y, uint8_t c) {
  parse_state_t *s = &y->state;
  uint8_t ret      = 0;
  switch (s->readState) {
  case NONE_STATE:
    if (c == YIVO_HEADER_0) {
      yivo_parser_reset(y);
      s->readState      = H0_STATE;
      y->buffer[y->p++] = c; // h0
    }
    else s->readState = NONE_STATE; // this state will reset
    break;
  case H0_STATE:
    if (c == YIVO_HEADER_1) {
      s->readState      = H1_STATE;
      y->buffer[y->p++] = c; // h1
    }
    else s->readState = NONE_STATE; // this state will reset
    break;
  case H1_STATE:
    s->readState      = S0_STATE;
    y->buffer[y->p++] = c; // size0
    break;
  case S0_STATE:
    s->readState    = S1_STATE;
    s->payload_size = (c << 8) | y->buffer[2];
    // printf("payload size: %d\n", (int)s->payload_size);
    y->buffer[y->p++] = c; // size1
    break;
  case S1_STATE:
    s->readState      = TYPE_STATE;
    y->buffer[y->p++] = c; // type or msg id
    s->buffer_msgid   = c;
    break;
  case TYPE_STATE:
    s->readState      = DATA_STATE;
    y->buffer[y->p++] = c; // data0
    break;
  case DATA_STATE:
    y->buffer[y->p++] = c; // data1-dataN
    // if ((buffer.size() - 5) == y->payload_size) y->readState = CS_STATE;
    // printf("payload size: %d %d\n", y->p, (int)s->payload_size);
    if ((y->p - 5) == s->payload_size) s->readState = CS_STATE;
    break;
  case CS_STATE:
    y->buffer[y->p++] = c; // checksum
    // check if cs is correct
    uint8_t cs = 0;
    // for (size_t i=2; i < buffer.size()-1; ++i) cs ^= y->buffer[i];
    for (size_t i = 2; i < s->payload_size + 5; ++i)
      cs ^= y->buffer[i];
    if (cs == c) {
      ret = s->buffer_msgid;
      // printf("good checksum\n");
    }
    // else printf(" %d != %d\n", (int)c, (int)cs);
    s->readState = NONE_STATE;
    break;
  }

  return ret;
}

yivopkt_t *yivo_parse_get(yivo_parser_t *y) {
  yivopkt_t *ret = (yivopkt_t *)malloc(sizeof(yivopkt_t));
  uint32_t size  = y->state.payload_size + 6;
  if (y->p < size) return NULL;
  uint32_t i       = y->p - size;
  ret->buffer      = &y->buffer[i];
  ret->buffer_size = size;
  return ret;
}