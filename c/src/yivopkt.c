#include "yivo/yivo.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define YIVO_H0       0
#define YIVO_H1       1
#define YIVO_LN       2
#define YIVO_HN       3
#define YIVO_ID       4
#define YIVO_CS       5
#define YIVO_PL       6
#define YIVO_OVERHEAD 6 // h0,h1,LN,HN,ID,CS

const uint8_t crc8_table[256] = {0x00, 0x07, 0x0E, 0x09, 0x1C, 0x1B, 0x12, 0x15, 0x38, 0x3F, 0x36, 0x31, 0x24, 0x23, 0x2A, 0x2D, 0x70, 0x77, 0x7E, 0x79, 0x6C, 0x6B, 0x62, 0x65, 0x48, 0x4F,
                                 0x46, 0x41, 0x54, 0x53, 0x5A, 0x5D, 0xE0, 0xE7, 0xEE, 0xE9, 0xFC, 0xFB, 0xF2, 0xF5, 0xD8, 0xDF, 0xD6, 0xD1, 0xC4, 0xC3, 0xCA, 0xCD, 0x90, 0x97, 0x9E, 0x99,
                                 0x8C, 0x8B, 0x82, 0x85, 0xA8, 0xAF, 0xA6, 0xA1, 0xB4, 0xB3, 0xBA, 0xBD, 0xC7, 0xC0, 0xC9, 0xCE, 0xDB, 0xDC, 0xD5, 0xD2, 0xFF, 0xF8, 0xF1, 0xF6, 0xE3, 0xE4,
                                 0xED, 0xEA, 0xB7, 0xB0, 0xB9, 0xBE, 0xAB, 0xAC, 0xA5, 0xA2, 0x8F, 0x88, 0x81, 0x86, 0x93, 0x94, 0x9D, 0x9A, 0x27, 0x20, 0x29, 0x2E, 0x3B, 0x3C, 0x35, 0x32,
                                 0x1F, 0x18, 0x11, 0x16, 0x03, 0x04, 0x0D, 0x0A, 0x57, 0x50, 0x59, 0x5E, 0x4B, 0x4C, 0x45, 0x42, 0x6F, 0x68, 0x61, 0x66, 0x73, 0x74, 0x7D, 0x7A, 0x89, 0x8E,
                                 0x87, 0x80, 0x95, 0x92, 0x9B, 0x9C, 0xB1, 0xB6, 0xBF, 0xB8, 0xAD, 0xAA, 0xA3, 0xA4, 0xF9, 0xFE, 0xF7, 0xF0, 0xE5, 0xE2, 0xEB, 0xEC, 0xC1, 0xC6, 0xCF, 0xC8,
                                 0xDD, 0xDA, 0xD3, 0xD4, 0x69, 0x6E, 0x67, 0x60, 0x75, 0x72, 0x7B, 0x7C, 0x51, 0x56, 0x5F, 0x58, 0x4D, 0x4A, 0x43, 0x44, 0x19, 0x1E, 0x17, 0x10, 0x05, 0x02,
                                 0x0B, 0x0C, 0x21, 0x26, 0x2F, 0x28, 0x3D, 0x3A, 0x33, 0x34, 0x4E, 0x49, 0x40, 0x47, 0x52, 0x55, 0x5C, 0x5B, 0x76, 0x71, 0x78, 0x7F, 0x6A, 0x6D, 0x64, 0x63,
                                 0x3E, 0x39, 0x30, 0x37, 0x22, 0x25, 0x2C, 0x2B, 0x06, 0x01, 0x08, 0x0F, 0x1A, 0x1D, 0x14, 0x13, 0xAE, 0xA9, 0xA0, 0xA7, 0xB2, 0xB5, 0xBC, 0xBB, 0x96, 0x91,
                                 0x98, 0x9F, 0x8A, 0x8D, 0x84, 0x83, 0xDE, 0xD9, 0xD0, 0xD7, 0xC2, 0xC5, 0xCC, 0xCB, 0xE6, 0xE1, 0xE8, 0xEF, 0xFA, 0xFD, 0xF4, 0xF3};

/**
 * @brief Calculates CRC-8 checksum (ATM polynomial) for a packet.
 * @param y Pointer to the ypkt_t packet. Must not be NULL.
 * @return The computed CRC-8 checksum, or 0 if y or y->data is NULL.
 * @note Includes size (LN, HN), message ID, and payload in the checksum.
 *       Uses CRC-8-ATM polynomial (x^8 + x^2 + x + 1, 0x07).
 */
static uint8_t yivo_calc_checksum(ypkt_t *y) {
  if (y == NULL || y->data == NULL) return 0;

  uint8_t crc = 0x00; // Initial value
  // Include size (LN, HN) and ID in checksum
  crc = crc8_table[crc ^ y->data[YIVO_LN]];
  crc = crc8_table[crc ^ y->data[YIVO_HN]];
  crc = crc8_table[crc ^ y->data[YIVO_ID]];
  // Process payload
  for (uint16_t i = YIVO_PL; i < y->size; i++) {
    crc = crc8_table[crc ^ y->data[i]];
  }
  return crc;
}

/**
 * @brief Creates a new ypkt_t packet with allocated buffer.
 * @param size Size of the payload (excluding header and checksum).
 * @return Pointer to the created ypkt_t, or NULL on allocation failure.
 * @note The total buffer size is size + YIVO_OVERHEAD (6 bytes for header and checksum).
 */
ypkt_t *ypkt_create(const uint8_t msgid, uint16_t size) {
  size += YIVO_OVERHEAD;
  ypkt_t *y = (ypkt_t *)calloc(1, sizeof(ypkt_t));
  if (y == NULL) return NULL;
  y->data = (uint8_t *)calloc(1, size);
  if (y->data == NULL) {
    free(y);
    return NULL;
  }
  y->size = size;
  if (msgid == 0) return NULL;
  y->msg_id = msgid;
  return y;
}

/**
 * @brief Frees a ypkt_t packet and its data buffer.
 * @param y Pointer to the ypkt_t to free. Can be NULL.
 * @return true if freed successfully, false if y is NULL.
 */
ypkt_t *ypkt_free(ypkt_t *y) {
  if (y == NULL) return NULL;
  if (y->data != NULL) free(y->data);
  free(y);
  return NULL;
}

/**
 * @brief Packs a payload into a ypkt_t with headers and checksum.
 * @param y Pointer to the ypkt_t to pack into. Must not be NULL.
 * @param msgid Message ID (1-255, 0 is invalid).
 * @param data Pointer to the payload data. Must not be NULL.
 * @param len Length of the payload data.
 * @return 0 on success, or error code (YIVO_PKT_NULL, YIVO_SRC_NULL, YIVO_PKT_ID_ERR, YIVO_SIZE_ERROR).
 * @note The packet buffer must be sized for len + YIVO_OVERHEAD.
 *       Assumes data is serialized in the sender's native endianness.
 */
int ypkt_pack(ypkt_t *y, uint8_t *data, const uint16_t len) {
  if ((y == NULL) || (y->data == NULL)) return YIVO_PKT_NULL;
  if (data == NULL) return YIVO_SRC_NULL;
  if (y->size != len + YIVO_OVERHEAD) {
    YIVO_DEBUG("ysize(%u) != len(%u)\n", y->size, len + YIVO_OVERHEAD);
    return YIVO_SIZE_ERROR;
  }

  y->data[YIVO_H0] = YIVO_HEADER_0;
  y->data[YIVO_H1] = YIVO_HEADER_1;
  y->data[YIVO_LN] = (uint8_t)(len & 0x00FF); // lo
  y->data[YIVO_HN] = (uint8_t)(len >> 8);     // hi
  y->data[YIVO_ID] = y->msg_id;
  y->data[YIVO_CS] = 0;

  memcpy(&y->data[YIVO_PL], data, len);

  y->data[YIVO_CS] = yivo_calc_checksum(y);

  return 0;
}

/**
 * @brief Unpacks the payload from a ypkt_t into a destination buffer.
 * @param y Pointer to the ypkt_t to unpack. Must not be NULL.
 * @param dst Pointer to the destination buffer to store the payload. Must not be NULL.
 * @param size Expected size of the payload.
 * @return 0 on success, or error code (YIVO_PKT_NULL, YIVO_SRC_NULL, YIVO_SIZE_ERROR, YIVO_PKT_HEADER_ERR, YIVO_PKT_ID_ERR, YIVO_PKT_CS_ERR).
 * @note Validates packet integrity before copying. Assumes dst is sized for the payload and matches sender's endianness.
 */
int ypkt_unpack(ypkt_t *y, void *dst, uint16_t size) {
  if (y == NULL || y->data == NULL) return YIVO_PKT_NULL;
  if (dst == NULL) return YIVO_SRC_NULL;

  // Validate packet
  int valid = ypkt_valid_msg(y);
  if (valid != 0) return valid;

  // Check payload size
  uint16_t payload_size = (y->data[YIVO_HN] << 8) | y->data[YIVO_LN];
  if (payload_size != size) return YIVO_SIZE_ERROR;

  // Copy payload to destination
  memcpy(dst, &y->data[YIVO_PL], payload_size);
  return 0;
}

/**
 * @brief Validates a ypkt_t packet for correct headers, size, and checksum.
 * @param y Pointer to the ypkt_t to validate. Must not be NULL.
 * @return 0 if valid, or error code (YIVO_PKT_NULL, YIVO_PKT_HEADER_ERR, YIVO_PKT_ID_ERR, YIVO_SIZE_ERROR, YIVO_PKT_CS_ERR).
 */
int ypkt_valid_msg(ypkt_t *y) {
  if ((y == NULL) || (y->data == NULL)) return YIVO_PKT_NULL;
  if ((y->data[0] != YIVO_HEADER_0) || (y->data[1] != YIVO_HEADER_1)) return YIVO_PKT_HEADER_ERR;
  if (y->data[YIVO_ID] == 0) return YIVO_PKT_ID_ERR;

  uint16_t msg_size = (y->data[YIVO_HN] << 8) | y->data[YIVO_LN];
  msg_size += YIVO_OVERHEAD;
  if (y->size != msg_size) return YIVO_SIZE_ERROR;

  uint8_t cs = yivo_calc_checksum(y);
  YIVO_DEBUG("CS: %u ?= %u\n", cs, y->data[YIVO_CS]);
  if (cs != y->data[YIVO_CS]) return YIVO_PKT_CS_ERR;

  return 0;
}