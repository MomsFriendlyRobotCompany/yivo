![](https://raw.githubusercontent.com/MomsFriendlyRobotCompany/yivo/main/docs/yivo.png)

# Yivo

## `yivopkt_t`

This is the serialized message that is sent across the wire.

```c
// get size of messages you want to send
size_t msg_size = sizeof(my_message);

// create a custom struct to hold message, this is reusable
// for this message type only
yivopkt_t *ymsg = ypkt_create(msg_size);
// pack message, id=10
ypkt_pack(ymsg, 10, &my_message, msg_size);

// use a method to send message like UDP, serial, etc
send(ymsg.data, ymsg.size);
```

## `yivo_parser_t`

This is the functions that parse incoming bytes and find the message
in the data stream.

```c
yivo_parser_t *pars = yivo_parse_create();

for (int i=0; i<BUFFER_SIZE; ++i) {
    uint8_t b = buffer[i];
    int msg_id = yivo_parse(pars, b);

    if (msg_id == 10) {
        my_msg_t m;
        yivo_parse_get(pars, &m, sizeof(m));
        // do something with m
    }
    if (msg_id == 11) // get message
    if (msg_id == 12) // etc ... 254 potiential msg ids
}
```

## Messages

You want to avoid padding, so things are easier to unpack. The very long, but
usefull command `__attribute__((packed))` does this. There is a simple define
(yivo message or `YMSG`) that inserts this and is shorter.

```c
// YMSG -> __attribute__((packed))
// this is shorter ... you don't have to use it
typedef struct YMSG {
  int a;
  int b;
} test_t;
```

## Build

For testing, you need to use `scruffy` and do `ln -s ~/github/scruffy scruffy`
in `scruffy/extlibs` folder.

## ToDo

Grok suggested:

- [ ] In `parse.c` incrementally calculate checksum
    - If checksum fails, reset parser and return 0
    - Enforce msg_id = 0 as an invalid, reset parser and return 0 
- [ ] Fix mix of snake case and cammel case
- [ ] Instead of XOR for checksum, upgrade to CRC-8
- [ ] Use a fixed buffer instead of a dynamic buffer for messages
- [ ] Add a function for parsing that accepts a fixed buffer in addition
    to the streaming byte-by-byte function
- [ ] Add ID and size to checksum

# MIT License

**Copyright (c) 2020 Mom's Friendly Robot Company**

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
