#include <stdint.h>
#include <stdio.h>

#define CRC8_POLYNOMIAL 0x07

// Function to generate CRC-8 lookup table
void generate8(uint8_t table[256]) {
  for (uint16_t i = 0; i < 256; ++i) {
    uint8_t crc = i;
    for (size_t j = 1; j < 8; j++) {
      crc = (crc & 0x80) ? (crc << 1) ^ CRC8_POLYNOMIAL : crc << 1;
    }
    table[i] = crc;
  }
}

#define CRC16_POLYNOMIAL 0x8005

// Function to generate CRC-16 lookup table
void generate16(uint16_t table[256]) {
  for (uint16_t i = 0; i < 256; ++i) {
    uint16_t crc = i;
    for (int j = 0; j < 8; ++j) {
      crc = (crc & 1) ? (crc >> 1) ^ CRC16_POLYNOMIAL : (crc >> 1);
    }
    table[i] = crc;
  }
}

int main() {
  // Example data
  // uint8_t data[] = "Hello, CRC!";
  uint16_t table[256];
  uint8_t table8[256];

  generate8(table8);

  // Print the result
  for (size_t i = 0; i < 16; ++i) {
    for (size_t j = 0; j < 16; ++j) {
      // printf("0x%02X,", table8[i*16+j]);
      printf("%u,", table8[i * 16 + j]);
    }
    printf("\n");
  }

  printf("--------------------------\n");

  generate16(table);

  // Print the result
  for (size_t i = 0; i < 16; ++i) {
    for (size_t j = 0; j < 16; ++j) {
      printf("0x%04X,", table[i * 16 + j]);
    }
    printf("\n");
  }

  /*
  crc = 0
  for byte in message:
    crcByte = byte^crc
    crc = table[crcByte]
  */

  return 0;
}