

#include <stdint.h>
#include <stdio.h>
#include <strings.h> // memset

#include <yivo/yivo.h>
#define BUFFER_SIZE   128
#define YIVO_NUM_MSGS 20

static uint8_t buffer[BUFFER_SIZE];

#define IBUFFER_SIZE 32
uint8_t ibuffer[IBUFFER_SIZE];

// typedef struct __attribute__((packed)) {
typedef struct YMSG {
  int a, b;
} test_t;

// typedef struct __attribute__((packed)) {
typedef struct YMSG {
  int i;
  float f;
  char c;
} test2_t;

int main() {
  bool ok;
  test_t t  = {.a = -10, .b = 300};
  ypkt_t *a = ypkt_create(sizeof(test_t));
  if (ypkt_pack(a, 10, (uint8_t *)&t, sizeof(t)) < 0) printf("bad pack\n");
  printf("a size: %u\n", a->size);

  test2_t tt = {999, -3.14, 'a'};
  ypkt_t *b  = ypkt_create(sizeof(test2_t));
  if (ypkt_pack(b, 20, (uint8_t *)&tt, sizeof(tt)) < 0) printf("bad pack\n");
  printf("b size: %u\n", b->size);

  printf("--------------\n");

  buffer[0] = 34;
  buffer[1] = 200;
  memcpy(&buffer[2], a->data, a->size);
  memcpy(&buffer[20], b->data, b->size);
  memcpy(&buffer[50], a->data, a->size);
  memcpy(&buffer[70], a->data, a->size);
  memcpy(&buffer[100], b->data, b->size);

  // ypars_t pars;
  // ypars_init(pars);
  ypars_t *pars = ypars_create(ibuffer, IBUFFER_SIZE);

  // bad data --------------------
  yivopars(pars, 1);
  yivopars(pars, '$');
  yivopars(pars, 72);
  yivopars(pars, '$');
  yivopars(pars, '$');
  yivopars(pars, 212);

  for (int i = 0; i < BUFFER_SIZE; ++i) {
    uint8_t byte = buffer[i];
    char ch      = ((byte > 32) && (byte < 127)) ? (char)byte : '-';
    // printf("%d: 0x%02X  %c\n", i, byte, ch);

    uint8_t mid = yivopars(pars, byte);
    if (mid == 10) {
      // printf("Found msg id: %d\n", (int)mid);
      test_t ans;
      int err = ypars_get(pars, (uint8_t *)&ans, sizeof(test_t));
      if (err < 0) printf("error: %d\n", err);
      else printf("test_t[%u]: %d %d\n", mid, ans.a, ans.b);
    } else if (mid == 20) {
      // printf("Found msg id: %d\n", (int)mid);
      test2_t ans;
      int err = ypars_get(pars, (uint8_t *)&ans, sizeof(test2_t));
      if (err < 0) printf("error: %d\n", err);
      else printf("test2_t[%u]: %d %f %c\n", mid, ans.i, ans.f, ans.c);
    }
  }

  // if (ypkt_unpack(msg2, &tt) == false) printf("bad unpack\n");

  // printf("tt: %d %d\n", tt.a, tt.b);
  // ypkt_free(msg);
  // ypkt_free(msg2);

  return 0;
}