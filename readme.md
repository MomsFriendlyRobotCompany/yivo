![](https://raw.githubusercontent.com/MomsFriendlyRobotCompany/yivo/main/docs/yivo.png)

# Yivo

[![Cpp](https://github.com/MomsFriendlyRobotCompany/yivo/actions/workflows/cpp.yaml/badge.svg)](https://github.com/MomsFriendlyRobotCompany/yivo/actions/workflows/cpp.yaml)
[![Python](https://github.com/MomsFriendlyRobotCompany/yivo/actions/workflows/python.yaml/badge.svg)](https://github.com/MomsFriendlyRobotCompany/yivo/actions/workflows/python.yaml)
![GitHub](https://img.shields.io/github/license/MomsFriendlyRobotCompany/yivo)

Trying to standardize the way I access sensors.

# API

| Part     | In Checksum | Type  | Description |
|----------|-------------|-------|-------------|
| Header   |   | `uint8_t[2]`    | default `$K` |
| Size     | x | `uint16_t`      | 0-65,535 bytes [L,H] => `(H << 8) | L` |
| Type     | x | `uint8_t`       | 256 message IDs |
| Data     | x | `uint8_t[Size]` | payload data array |
| Checksum |   | `uint8_t`       | XOR of size, type, and data bytes |

| 0 | 1 | 2 | 3 | 4 | ... | -1 |
|---|---|---|---|---|-----|----|
|`$`|`K`| L | H | T | ... | checksum |

## Todo

- [ ] Should I move to a 16b checksum?
- [ ] Should I use a 16b message id instead of 8b?
- [x] Remove messages from library. This is just a binary packer and
      not dependant on the message format other than, it is a `struct`
- [x] c++: Make a header only library
- [ ] Generate C and Python messages from a template instead of writing independently
- [ ] c++: rename `YivoPack_t` to something else like `ypkt_t`
    - [ ] Add method `bool has_valid_chksum()`? Class could calculate checksum and then compare
    to its checksum
- [ ] c++: Should I move away from `std::vector` and to a fixed array size, since on create of
    a message, I know the complete size ... it isn't a mystery at that point
    - would now have to use `new` and `delete` which aren't my favorite

## Python

```python
from yivo import Yivo, MsgInfo

# make some messages we want to send/receive
A = namedtuple("A","x y")      # id = 1
B = namedtuple("B", "x y z t") # id = 2

msgdb = MsgInfo()
msgdb[1] = ("2f", A) # 2 floats
msgdb[2] = ("4f", B) # 4 floats

yivo = Yivo(msgdb)
pkt = yivo.pack(1, A(1.,2.))

id = 0
while id == 0:
    b = serial_read() # get a byte from somewhere
    id = yivo.parse(b) # 0 or msg_id

err, msg = yivo.unpack() # err > 0 if failure to unpack
```

## C++

```cpp
#include <yivo.hpp>

// some messages
struct A {int a;}; // id 1
struct B {int b;}; // id 2

Yivo yivo;

YivoPack_t pkt = yivo.pack(1, A{1}, sizeof(A));

uint8_t id = 0;
while (id == 0) {
    uint8_t b = serial_read(); // get a byte from somewhere
    id = yivo.parse(b);
}
if (id == 1) A a = yivo.unpack<A>();
else if (id == 2) B b = yivo.unpack<B>();
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
