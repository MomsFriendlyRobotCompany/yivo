![](https://raw.githubusercontent.com/MomsFriendlyRobotCompany/yivo/main/docs/yivo.png)

# Yivo


[![Cpp](https://github.com/MomsFriendlyRobotCompany/yivo/actions/workflows/cpp.yaml/badge.svg)](https://github.com/MomsFriendlyRobotCompany/yivo/actions/workflows/cpp.yaml)
[![Python](https://github.com/MomsFriendlyRobotCompany/yivo/actions/workflows/python.yaml/badge.svg)](https://github.com/MomsFriendlyRobotCompany/yivo/actions/workflows/python.yaml)
![GitHub](https://img.shields.io/github/license/MomsFriendlyRobotCompany/yivo)


Trying to standardize the way I access sensors.

Message format:

```
[ 0, 1, 2, 3,4, ..., -1]
[h0,h1,LN,HN,T, ..., CS]
Header: h0, h1
N = (HN << 8) + LN, max data bytes is 65,536 Bytes
  HN: High Byte
  LN: Low Byte
T: packet type or MsgID
CS: simple checksum
```

## Python

```python
from yivo import Yivo, make_Struct

# make some messages we want to send/receive
A = namedtuple("A","x y")
B = namedtuple("B", "x y z t")

msgdb = {
    1: (make_Struct("2f"), A), # 2 floats
    2: (make_Struct("4f"), B)  # 4 floats
}
yivo = Yivo(msgdb)
pkt = yivo.pack(1, A(1.,2.))

id = 0
while id == 0:
    b = serial_read() # get a byte from somewhere
    id = yivo.parse(b) # 0 or msg_id

err, msg = yivo.unpack() # err > 0 if failure to unpack
```

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
