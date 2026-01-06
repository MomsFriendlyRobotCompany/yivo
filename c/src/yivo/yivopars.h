#pragma once

#include "yivo/yivopkt.h"
#include <stdbool.h>
#include <stdint.h>
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

typedef struct {
  uint8_t *data;
  uint16_t size;
  uint16_t index;
} buffer_info_t;

typedef struct {
  uint8_t id;
  uint8_t cs;                // Stored checksum
  uint8_t computed_crc;      // Incrementally computed CRC
  uint8_t ln, hn;        // Stored size low/high, ID for CRC
  uint16_t size;
} msg_info_t;

/**
 * @brief Parser state for streaming Yivo packet deserialization.
 */
typedef struct {
  uint8_t *payload;          // User-supplied fixed buffer for payload
  uint16_t payload_size;     // Size of current payload
  uint16_t max_payload_size; // Maximum size of the user-supplied buffer
  read_state_e readState;         // Parser state
  uint8_t buffer_msgid;      // Detected message ID
  uint16_t index;            // Index into payload
  uint8_t cs;                // Stored checksum
  uint8_t computed_crc;      // Incrementally computed CRC
  uint8_t ln, hn, id;        // Stored size low/high, ID for CRC
  // uint32_t found_loc;        // index where message was found
} ypars_t;

/**
 * @brief Creates a new ypars_t with a user-supplied fixed-size payload buffer.
 * @param buffer User-supplied buffer for payload.
 * @param max_size Maximum size of the buffer.
 * @return Pointer to the created ypars_t, or NULL on allocation failure.
 * @note Optimized for embedded systems; rejects messages larger than max_size.
 */
ypars_t *ypars_create(uint16_t max_size);

/**
 * @brief Resets the parser state.
 * @param y Pointer to the ypars_t.
 */
// void ypars_reset(ypars_t *y);

/**
 * @brief Parses a single byte in streaming fashion, updating state.
 * @param y Pointer to the ypars_t.
 * @param c The byte to parse.
 * @return Message ID if a complete valid message is found, 0 otherwise.
 * @note Validates message ID (>0), checks size against max_payload_size, and
 * validates CRC incrementally.
 */
uint8_t ypars_stream(ypars_t *y, uint8_t c);
uint8_t ypars_buffer(ypars_t *y, uint8_t *buffer, uint32_t buffer_size,
                     uint32_t *loc);
// uint8_t ypars_buffer(ypars_t *y, uint8_t *buffer, uint32_t buffer_size);
// uint8_t ypars_find(ypars_t *y);

/**
 * @brief Copies the parsed payload to a user buffer.
 * @param y Pointer to the ypars_t.
 * @param buffer User buffer to copy to.
 * @param size Expected size of the payload.
 * @return 0 on success, error code on failure (YIVO_PARSER_NULL, YIVO_PKT_NULL,
 * YIVO_SIZE_ERROR).
 */
int32_t ypars_get(ypars_t *y, uint8_t *buffer, uint16_t size);

/**
 * @brief Frees the ypars_t.
 * @param y Pointer to the ypars_t.
 */
ypars_t *ypars_free(ypars_t *y);