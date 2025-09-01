#include "yivo/yivo.h"
// #include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// #define YIVO_HEADER_0 '$'
// #define YIVO_HEADER_1 'K'
#define YIVO_H0       0
#define YIVO_H1       1
#define YIVO_LN       2
#define YIVO_HN       3
#define YIVO_ID       4
#define YIVO_CS       5
#define YIVO_PL       6
#define YIVO_OVERHEAD 6 // h0,h1,LN,HN,ID, ..., CS

#if 0
  #define DEBUG(format, ...)         \
    do {                             \
      printf(format, ##__VA_ARGS__); \
    } while (0)
#else
  #define DEBUG(format, ...)
#endif

yivopkt_t *ypkt_create(uint16_t size) {
  size += YIVO_OVERHEAD;
  yivopkt_t *y = (yivopkt_t *)calloc(1, sizeof(yivopkt_t));
  y->data      = (uint8_t *)calloc(1, size);
  y->size      = size;
  return y;
}

bool ypkt_free(yivopkt_t *y) {
  if (y == NULL) return false;
  if (y->data != NULL) free(y->data);
  free(y);
  y = NULL;
  return true;
}

static uint8_t yivo_calc_checksum(yivopkt_t *y) {
  if (y == NULL) return 0;
  // XOR all payload bytes
  uint32_t cs = 0;
  for (uint16_t i = YIVO_PL; i < y->size; ++i)
    cs ^= y->data[i];
  return (uint8_t)(cs & 0x000000FF);
}

/*
msgid: message ID (1-255), 0 is not allowed
data: buffer containing payload data to be sent
sz: size of payload to be sent, yivopkt_t.size() == (size + 6)
*/
int ypkt_pack(yivopkt_t *y, const uint8_t msgid, uint8_t *data, const uint16_t len) {
  if ((y == NULL) || (y->data == NULL)) return YIVO_PKT_NULL;
  if (data == NULL) return YIVO_SRC_NULL;
  if (y->size != len + YIVO_OVERHEAD) {
    DEBUG("ysize(%u) != len(%u)\n", y->size, len + YIVO_OVERHEAD);
    return YIVO_SIZE_ERROR;
  }

  y->data[YIVO_H0] = YIVO_HEADER_0;
  y->data[YIVO_H1] = YIVO_HEADER_1;
  y->data[YIVO_LN] = (uint8_t)(len & 0x00FF); // lo
  y->data[YIVO_HN] = (uint8_t)(len >> 8);     // hi
  y->data[YIVO_ID] = msgid;
  y->data[YIVO_CS] = 0;

  memcpy(&y->data[YIVO_PL], data, len);

  y->data[YIVO_CS] = yivo_calc_checksum(y);
  // y->size = len + YIVO_OVERHEAD;

  return 0;
}

// int32_t ypkt_unpack(yivopkt_t *y, void *dst) {
//   if (y->buffer == NULL || y->buffer_size < YIVO_OVERHEAD) return -1;

//   int32_t size    = y->buffer_size - YIVO_OVERHEAD;
//   uint8_t *buffer = &y->buffer[YIVO_PL];
//   memcpy(dst, buffer, size);
//   return size;
// }

int ypkt_valid_msg(yivopkt_t *y) {
  if ((y == NULL) || (y->data == NULL)) return YIVO_PKT_NULL;
  if ((y->data[0] != YIVO_HEADER_0) || (y->data[1] != YIVO_HEADER_1)) return YIVO_PKT_HEADER_ERR;
  if (y->data[YIVO_ID] == 0) return YIVO_PKT_ID_ERR;

  uint16_t msg_size = (y->data[YIVO_HN] << 8) | y->data[YIVO_LN];
  msg_size += YIVO_OVERHEAD;
  if (y->size != msg_size) return YIVO_SIZE_ERROR;

  uint8_t cs = yivo_calc_checksum(y);
  DEBUG("CS: %u ?= %u\n", cs, y->data[YIVO_CS]);
  if (cs != y->data[YIVO_CS]) return YIVO_PKT_CS_ERR;

  return 0;
}