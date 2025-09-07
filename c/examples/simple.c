#include <stdint.h>
#include <stdio.h>
#include <strings.h> // memset

#include <yivo/yivo.h>

typedef struct YMSG {
  int a, b;
} test_t;

#define BUFFER_SIZE 32
uint8_t buffer[BUFFER_SIZE];

int main() {
  bool ok;
  test_t t = {-10, 300};
  test_t tt;
  uint32_t size = sizeof(test_t);

  ypkt_t *msg = ypkt_create(size);

  ypars_t *pars = ypars_create(buffer, BUFFER_SIZE);

  int err = ypkt_pack(msg, 10, (uint8_t *)&t, sizeof(t));
  if (err < 0) printf("bad pack: %d\n", err);
  printf("msg size: %u\n", msg->size);
  printf("valid message: %d\n", ypkt_valid_msg(msg));

  // bad data --------------------
  yivopars(pars, 1);
  yivopars(pars, '$');
  yivopars(pars, 72);
  yivopars(pars, '$');
  yivopars(pars, '$');
  yivopars(pars, 212);

  for (int i = 0; i < msg->size + 6; ++i) {
    uint8_t b = msg->data[i];
    // printf("0x%02X\n", b);
    uint8_t mid = yivopars(pars, b);
    if (mid > 0) {
      printf("Found msg id: %u\n", mid);
      int err = ypars_get(pars, (uint8_t *)&tt, sizeof(test_t));
      if (err < 0) printf("error: %d\n", err);
      break;
    }
  }

  // if (ypkt_unpack(msg2, &tt) == false) printf("bad unpack\n");

  printf("test_t size: %d\n", (int)size);
  printf("t: %d %d\n", t.a, t.b);
  printf("tt: %d %d\n", tt.a, tt.b);
  ypkt_free(msg);

  return 0;
}