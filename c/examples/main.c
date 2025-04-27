#include <stdint.h>
#include <stdio.h>
#include <strings.h> // memset

#include <yivo/yivo.h>

typedef struct {
  int a, b;
} test_t;

int main() {
  bool ok;
  test_t t = {-10, 300};
  test_t tt;
  uint32_t size = sizeof(test_t);
  printf("test_t size: %d\n", (int)size);

  yivopkt_t *msg = yivo_init(size);
  printf("msg buffer: %d\n", msg->buffer_size);

  yivopkt_t *msg2 = NULL;
  yivo_parser_t pars;

  // if (yivo_init(&msg, size) == false) printf("bad inint\n");
  if (yivo_pack(msg, 10, (uint8_t *)&t, sizeof(t)) == false) printf("bad pack\n");

  yivo_parse_init(&pars, 64);

  // bad data --------------------
  yivo_parse(&pars, 1);
  yivo_parse(&pars, '$');
  yivo_parse(&pars, 72);
  yivo_parse(&pars, '$');
  yivo_parse(&pars, '$');
  yivo_parse(&pars, 212);

  for (int i = 0; i < msg->buffer_size; ++i) {
    printf("%d\n", i);
    uint8_t b   = msg->buffer[i];
    uint8_t mid = yivo_parse(&pars, b);
    if (mid > 0) {
      printf("Found msg id: %d\n", (int)mid);
      msg2 = yivo_parse_get(&pars);
      break;
    }
  }

  if (yivo_unpack(msg2, &tt) == false) printf("bad unpack\n");

  printf("tt: %d %d\n", tt.a, tt.b);
  yivo_free(msg);
  yivo_free(msg2);

  return 0;
}