![](https://raw.githubusercontent.com/MomsFriendlyRobotCompany/yivo/main/docs/yivo.png)

# Yivo

[![Python](https://github.com/MomsFriendlyRobotCompany/yivo/actions/workflows/python.yaml/badge.svg)](https://github.com/MomsFriendlyRobotCompany/yivo/actions/workflows/python.yaml)
![GitHub](https://img.shields.io/github/license/MomsFriendlyRobotCompany/yivo)

Trying to standardize the way I access sensors.

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


## API

| Part     | In Checksum | Type  | Description |
|----------|-------------|-------|-------------|
| Header   |   | `uint8_t[2]`    | default `$K` |
| Size     | x | `uint16_t`      | 0-65,535 bytes, stored as [L,H] => `(H << 8) | L` |
| Type     | x | `uint8_t`       | 255 message IDs, 0 is not allowed as an ID |
| Data     | x | `uint8_t[Size]` | payload data array |
| Checksum |   | `uint8_t`       | XOR of size, type, and data bytes |

| 0 | 1 | 2 | 3 | 4 | ... | -1 |
|---|---|---|---|---|-----|----|
|`$`|`K`| L | H | T | ... | checksum |

## gentools

Generate messages for python and C/C++.

| Type     | Bytes | Format | Python | C/C++ |
|----------|---|-----|-------|---------------|
| `uint8`  | 1 | `B` | `int` | `uint8_t`
| `uint16` | 2 | `H` | `int` | `uint16_t`
| `uint32` | 4 | `I` | `int` | `uint32_t`
| `uint64` | 8 | `Q` | `int` | `uint64_t`
| `int8`   | 1 | `b` | `int` | `int8_t`
| `int16`  | 2 | `h` | `int` | `int16_t`
| `int32`  | 4 | `i` | `int` | `int32_t`
| `int64`  | 8 | `q` | `int` | `int64_t`
| `float`  | 4 | `f` | `float` | `float`
| `double` | 8 | `d` | `float` | `double`


```
# comment ...
# comment ...

float     a # comment about var
uint8     b
int16[12] c

<enum something uint16_t
bob=0
tom=2
jerry=32
enum>
```

```json
{
    "namespace": "foobar",
    "license": "MIT Kevin Walchko (c) 2023",
    "output": "test",
    "1": "messages/vec.yivo",
    "2": "messages/quat.yivo",
    "4": "messages/imu.yivo",
    "5": "messages/cal.yivo"
}
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
