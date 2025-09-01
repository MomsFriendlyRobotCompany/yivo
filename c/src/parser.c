#include "yivo/yivo.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h> // memcpy, memset

#if 0
  #define DEBUG(format, ...)                                                                                                                                                                           \
    do {                                                                                                                                                                                               \
      printf(format, ##__VA_ARGS__);                                                                                                                                                                   \
    } while (0)
#else
  #define DEBUG(format, ...)
#endif

typedef enum : uint8_t {
  H0_STATE,   // 1
  H1_STATE,   // 2
  SZ0_STATE,  // 3
  SZ1_STATE,  // 4
  ID_STATE,   // 5
  CS_STATE,   // 6
  DATA_STATE, // 7
} read_state_e;

void yivo_parse_reset(yivo_parser_t *y) {
  // printf(">> yp_reset, pl: %p\n", y->payload);
  if (y->payload != NULL) free(y->payload);
  y->payload = NULL;
  // printf(">> yp_reset, pl: %p\n", y->payload);
  y->payload_size = 0;
  y->readState    = H0_STATE;
  y->buffer_msgid = 0;
  y->index        = 0;
}

yivo_parser_t *yivo_parse_create() {
  yivo_parser_t *y = (yivo_parser_t *)calloc(1, sizeof(yivo_parser_t));
  DEBUG(">> parser_create payload: %p size: %u\n", y->payload, y->payload_size);
  return y;
}

//
// Read in one byte at a time, yivo keeps track of the state until it
// finds a good message.
// Returns: valid message or 0
uint8_t yivo_parse(yivo_parser_t *y, uint8_t c) {
  uint8_t *buff = y->payload;

  switch (y->readState) {
  case H0_STATE:
    if (c == YIVO_HEADER_0) {
      yivo_parse_reset(y);
      y->readState = H1_STATE;
      DEBUG("START -----------\n");
      DEBUG("H0: %c\n", (char)c);
    }
    // else y->readState = H0_STATE; // this state will reset
    break;
  case H1_STATE:
    if (c == YIVO_HEADER_1) {
      y->readState = SZ0_STATE;
      DEBUG("H1: %c\n", (char)c);
    } else y->readState = H0_STATE; // this state will reset
    break;
  case SZ0_STATE:
    y->readState    = SZ1_STATE;
    y->payload_size = c;
    break;
  case SZ1_STATE:
    y->readState = ID_STATE;
    y->payload_size |= (c << 8);
    if (y->payload != NULL) free(y->payload);
    y->payload = (uint8_t *)calloc(1, y->payload_size);
    DEBUG("SIZE: %u y->payload: %p\n", y->payload_size, y->payload);
    break;
  case ID_STATE:
    y->readState    = CS_STATE;
    y->buffer_msgid = c;
    DEBUG("ID: %u\n", c);
    break;
  case CS_STATE:
    y->readState = DATA_STATE;
    y->cs        = c;
    y->index     = 0;
    DEBUG("CS: %u\n", c);
    break;
  case DATA_STATE:
    buff[y->index++] = c; // data1-dataN
    DEBUG("DATA[%u]: 0x%02X\n", y->index, c);
    if ((y->index - 0) == y->payload_size) {
      y->readState = H0_STATE;
      return y->buffer_msgid;
    }
    break;
  }

  return 0;
}

// - copies parser.payload over to buffer
// - frees parser.payload
int32_t yivo_parse_get(yivo_parser_t *y, uint8_t *buffer, uint16_t size) {
  if (buffer == NULL) return YIVO_PARSER_NULL;
  if (y->payload == NULL) return YIVO_PKT_NULL;
  if (size != y->payload_size) return YIVO_SIZE_ERROR;

  memcpy(buffer, y->payload, size);
  free(y->payload);
  y->payload = NULL;
  DEBUG(">> parse_get y->pl: %p\n", y->payload);

  return 0;
}

// bool yivo_parse_init(yivo_parser_t *y) {
//   if (y == NULL) return false;
//   yivo_parse_reset(y);
//   DEBUG(">> parser_create payload: %p size: %u\n", y->payload, y->payload_size);

//   return true;
// }