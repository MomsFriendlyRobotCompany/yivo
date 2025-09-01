#include <stdint.h>
#include <stdio.h>
#include <strings.h> // memset

#include <yivo/yivo.h>

typedef struct YMSG {
  int a, b;
} test_t;

int main() {
  bool ok;
  test_t t = {-10, 300};
  test_t tt;
  uint32_t size = sizeof(test_t);

  yivopkt_t *msg = ypkt_create(size);

  yivo_parser_t *pars = yivo_parse_create();

  int err = ypkt_pack(msg, 10, (uint8_t *)&t, sizeof(t));
  if (err < 0) printf("bad pack: %d\n", err);
  printf("msg size: %u\n", msg->size);
  printf("valid message: %d\n", ypkt_valid_msg(msg));

  // bad data --------------------
  yivo_parse(pars, 1);
  yivo_parse(pars, '$');
  yivo_parse(pars, 72);
  yivo_parse(pars, '$');
  yivo_parse(pars, '$');
  yivo_parse(pars, 212);

  for (int i = 0; i < msg->size + 6; ++i) {
    uint8_t b = msg->data[i];
    // printf("0x%02X\n", b);
    uint8_t mid = yivo_parse(pars, b);
    if (mid > 0) {
      printf("Found msg id: %u\n", mid);
      int err = yivo_parse_get(pars, (uint8_t *)&tt, sizeof(test_t));
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