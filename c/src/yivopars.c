#include "yivo/yivo.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h> // memcpy, memset

typedef enum : uint8_t {
  H0_STATE,   // Header byte 0
  H1_STATE,   // Header byte 1
  SZ0_STATE,  // Size low byte
  SZ1_STATE,  // Size high byte
  ID_STATE,   // Message ID
  CS_STATE,   // Checksum
  DATA_STATE, // Payload data
} read_state_e;

ypars_t *ypars_create(uint16_t max_size) {
  // if (buffer == NULL || max_size == 0) return NULL;
  ypars_t *y = (ypars_t *)calloc(1, sizeof(ypars_t));
  if (y == NULL) return NULL;
  y->payload          = (uint8_t *)calloc(1, max_size);
  y->max_payload_size = max_size;
  y->readState        = H0_STATE;
  YIVO_DEBUG(">> parser_create payload: %p max_size: %u\n", y->payload, y->max_payload_size);
  return y;
}

ypars_t *ypars_free(ypars_t *y) {
  if (y == NULL) return NULL;
  if (y->payload != NULL) {
    free(y->payload);
    y->payload = NULL;
  }
  free(y);
  // printf("y1 = %p\n", y);
  // y = NULL;
  // printf("y2 = %p\n", y);
  return NULL;
}

void ypars_reset(ypars_t *y) {
  if (y == NULL) return;
  y->payload_size = 0;
  y->readState    = H0_STATE;
  y->buffer_msgid = 0;
  y->index        = 0;
  y->cs           = 0;
  y->computed_crc = 0;
  y->ln           = 0;
  y->hn           = 0;
  y->id           = 0;
}

uint8_t ypars_stream(ypars_t *y, uint8_t c) {

  if (y == NULL) return 0;

  switch (y->readState) {
  case H0_STATE:
    if (c == YIVO_HEADER_0) {
      ypars_reset(y);
      y->readState = H1_STATE;
      YIVO_DEBUG("START -----------\n");
      YIVO_DEBUG("H0: %c\n", (char)c);
    }
    break;
  case H1_STATE:
    if (c == YIVO_HEADER_1) {
      y->readState = SZ0_STATE;
      YIVO_DEBUG("H1: %c\n", (char)c);
    } else {
      y->readState = H0_STATE;
    }
    break;
  case SZ0_STATE:
    y->ln           = c;
    y->payload_size = c;
    y->readState    = SZ1_STATE;
    break;
  case SZ1_STATE:
    y->hn = c;
    y->payload_size |= (c << 8);
    if (y->payload_size > y->max_payload_size) {
      YIVO_DEBUG("Payload size %u exceeds max %u\n", y->payload_size, y->max_payload_size);
      ypars_reset(y);
      return 0;
    }
    if (y->payload == NULL) { // safety check
      ypars_reset(y);
      return 0;
    }
    YIVO_DEBUG("SIZE: %u y->payload: %p\n", y->payload_size, y->payload);
    y->readState = ID_STATE;
    break;
  case ID_STATE:
    if (c == 0) {
      YIVO_DEBUG("Invalid message ID 0\n");
      ypars_reset(y);
      return 0;
    }
    y->id           = c;
    y->buffer_msgid = c;
    YIVO_DEBUG("ID: %u\n", c);
    y->readState = CS_STATE;
    break;
  case CS_STATE:
    y->cs    = c;
    y->index = 0;
    YIVO_DEBUG("CS: %u\n", c);
    // Start CRC computation with LN, HN, ID
    uint8_t crc     = 0x00;
    crc             = crc8_table[crc ^ y->ln];
    crc             = crc8_table[crc ^ y->hn];
    crc             = crc8_table[crc ^ y->id];
    y->computed_crc = crc;
    y->readState    = DATA_STATE;
    break;
  case DATA_STATE:
    if (y->payload == NULL) { // safety check
      ypars_reset(y);
      return 0;
    }
    y->payload[y->index] = c;
    y->computed_crc      = crc8_table[y->computed_crc ^ c];
    YIVO_DEBUG("DATA[%u]: 0x%02X\n", y->index, c);
    y->index++;
    if (y->index == y->payload_size) {
      if (y->computed_crc != y->cs) {
        YIVO_DEBUG("Checksum mismatch: computed %u != stored %u\n", y->computed_crc, y->cs);
        ypars_reset(y);
        return 0;
      }
      y->readState = H0_STATE;
      return y->buffer_msgid;
    }
    break;
  }

  return 0;
}

uint8_t ypars_buffer(ypars_t *y, uint8_t *msg_buffer, uint32_t buffer_size, uint32_t *loc) {
  if (msg_buffer == NULL) return YIVO_BUFFER_NULL;
  if (y == NULL || y->payload == NULL) return YIVO_PARSER_NULL;

  for (int i = 0; i < buffer_size; ++i) {
    uint8_t b = msg_buffer[i];
    *loc += 1;
    uint8_t msg_id = ypars_stream(y, b);
    if (msg_id > 0) return msg_id;
  }

  return 0;
}

int32_t ypars_get(ypars_t *y, uint8_t *buffer, uint16_t size) {
  if (buffer == NULL) return YIVO_BUFFER_NULL;
  if (y == NULL || y->payload == NULL) return YIVO_PARSER_NULL;
  if (size != y->payload_size) return YIVO_SIZE_ERROR;

  memcpy(buffer, y->payload, size);
  YIVO_DEBUG(">> parse_get y->pl: %p\n", y->payload);

  return 0;
}