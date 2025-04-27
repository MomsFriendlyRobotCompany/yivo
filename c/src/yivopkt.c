#include "yivo/yivopkt.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

// bool yivo_init(yivopkt_t *y, uint16_t size) {
//   if (y == NULL) return false;
//   size += 6; // add in overhead
//   y->buffer = (uint8_t*)malloc(size);
//   y->buffer_size = size;
//   return true;
// }

yivopkt_t *yivo_init(uint16_t size) {
  yivopkt_t *y = (yivopkt_t *)malloc(sizeof(yivopkt_t));
  if (y == NULL) return NULL;
  size += 6; // add in overhead
  y->buffer = (uint8_t *)malloc(size);
  if (y->buffer == NULL) return NULL;
  y->buffer_size = size;
  return y;
}

bool yivo_free(yivopkt_t *y) {
  if (y == NULL) return false;
  if (y->buffer != NULL) free(y->buffer);
  free(y);
  return true;
}

static uint8_t yivo_calc_checksum(yivopkt_t *y) {
  // XOR all bytes EXCEPT header and checksum
  uint32_t cs = 0;
  for (uint16_t i = 2; i < (y->buffer_size - 1); ++i)
    cs ^= y->buffer[i];
  return (uint8_t)(cs & 0x000000FF);
}

/*
msgid: message ID (1-255), 0 is not allowed
data: buffer containing payload data to be sent
sz: size of payload to be sent, yivopkt_t.size() == (payload_size + 6)
*/
bool yivo_pack(yivopkt_t *y, const uint8_t msgid, uint8_t *data, const uint16_t len) {
  if (y == NULL) return false;
  if (data == NULL) return false;
  if (y->buffer_size < len + 6) return false;
  // resize(len+YIVO_OVERHEAD);

  y->buffer[0]       = YIVO_HEADER_0;
  y->buffer[1]       = YIVO_HEADER_1;
  y->buffer[YIVO_LN] = (uint8_t)(len & 0x00FF); // lo
  y->buffer[YIVO_HN] = (uint8_t)(len >> 8);     // hi
  y->buffer[YIVO_ID] = msgid;

  memcpy(&y->buffer[YIVO_PL], data, len);
  y->buffer[len + 5] = yivo_calc_checksum(y);

  return true;
}

int32_t yivo_unpack(yivopkt_t *y, void *dst) {
  if (y->buffer == NULL || y->buffer_size < YIVO_OVERHEAD) return -1;

  int32_t size    = y->buffer_size - YIVO_OVERHEAD;
  uint8_t *buffer = &y->buffer[YIVO_PL];
  memcpy(dst, buffer, size);
  return size;
}

bool yivo_valid_msg(yivopkt_t *y) {
  if (y == NULL) return false;
  if (y->buffer == NULL) return false;
  // if (y->buffer_size < YIVO_OVERHEAD) return false;
  if ((y->buffer[0] != YIVO_HEADER_0) || (y->buffer[1] != YIVO_HEADER_1)) return false;
  // if (y->buffer[YIVO_ID] == 0) return false;

  uint16_t msg_size = (y->buffer[YIVO_HN] << 8) | y->buffer[YIVO_LN];
  if ((y->buffer_size - YIVO_OVERHEAD) != msg_size) return false;

  uint8_t cs   = yivo_calc_checksum(y);
  uint8_t mycs = y->buffer[y->buffer_size - 1];
  if (mycs != cs) return false;

  return true;
}