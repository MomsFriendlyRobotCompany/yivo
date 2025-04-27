![](https://raw.githubusercontent.com/MomsFriendlyRobotCompany/yivo/main/docs/yivo.png)

# Yivo

- `yivopkt_t`: packs/unpacks (or encodes/decodes) c `structs` for transmission
    - `uint8_t* data()`: get point to buffer
    - `uint16_t size()`: get size of buffer
    - `uint8_t msg_id()`: get message id
    - `void pack(id,*data,size)`: encode a `struct` to binary
    - `T unpack<T>()`: decode a binary message to a `struct`
- `Yivo`: basically a state machine for reading in serial data and determining when you have a message to read
    - `uint8_t parse(c)`: read in a byte at a time and return an ID or 0
    - `void get_packet(pkt)`: copy binary message to `yivopkt_t`

```cpp
#include <yivo.hpp>

// some messages
struct A {int a;}; // id 1
struct B {int b;}; // id 2

Yivo yivo;

yivopkt_t pkt;
pkt.pack(1, A{1}, sizeof(A));

serial_send(pkt.data(), pkt.size()); // somehow send a message

uint8_t id = 0;
while (id == 0) {
    uint8_t b = serial_read(); // get a byte from somewhere
    id = yivo.parse(b);
}
yivopkt_t rep;
yivo.get_packet(rep);

// then decode the received message
if (id == 1) A a = rep.unpack<A>();
else if (id == 2) B b = rep.unpack<B>();
```

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
