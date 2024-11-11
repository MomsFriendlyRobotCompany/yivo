![](https://raw.githubusercontent.com/MomsFriendlyRobotCompany/yivo/main/docs/yivo.png)

# Yivo

[![Cpp](https://github.com/MomsFriendlyRobotCompany/yivo/actions/workflows/cpp.yaml/badge.svg)](https://github.com/MomsFriendlyRobotCompany/yivo/actions/workflows/cpp.yaml)
[![Python](https://github.com/MomsFriendlyRobotCompany/yivo/actions/workflows/python.yaml/badge.svg)](https://github.com/MomsFriendlyRobotCompany/yivo/actions/workflows/python.yaml)
![GitHub](https://img.shields.io/github/license/MomsFriendlyRobotCompany/yivo)
[![Latest Version](https://img.shields.io/pypi/v/yivo.svg)](https://pypi.python.org/pypi/yivo/)
[![image](https://img.shields.io/pypi/pyversions/yivo.svg)](https://pypi.python.org/pypi/yivo)
[![image](https://img.shields.io/pypi/format/yivo.svg)](https://pypi.python.org/pypi/yivo)
![PyPI - Downloads](https://img.shields.io/pypi/dm/yivo?color=aqua)


Trying to standardize the way I access serial data from sensors and stuff.

- Languages
    - C++
    - Python

## API

| Part     | In Checksum | Type  | Description                                       |
|----------|-------------|-------|---------------------------------------------------|
| Header   |   | `uint8_t[2]`    | default `$K`                                      |
| Size     | x | `uint16_t`      | 0-65,535 bytes, stored as [L,H] => `(H << 8) | L` |
| Type     | x | `uint8_t`       | 255 message IDs, `0` is not allowed as an ID      |
| Data     | x | `uint8_t[Size]` | payload data array                                |
| Checksum |   | `uint8_t`       | XOR of size, type, and data bytes                 |

| 0 | 1 | 2 | 3 | 4 | ... | -1 |
|---|---|---|---|---|-----|----|
|`$`|`K`| L | H | T | ... | checksum |

## Byte Conversions

| Type     | Bytes | Format | Python | C/C++ |
|----------|---|-----|---------|---------------|
| `uint8`  | 1 | `B` | `int`   | `uint8_t`
| `uint16` | 2 | `H` | `int`   | `uint16_t`
| `uint32` | 4 | `I` | `int`   | `uint32_t`
| `uint64` | 8 | `Q` | `int`   | `uint64_t`
| `int8`   | 1 | `b` | `int`   | `int8_t`
| `int16`  | 2 | `h` | `int`   | `int16_t`
| `int32`  | 4 | `i` | `int`   | `int32_t`
| `int64`  | 8 | `q` | `int`   | `int64_t`
| `float`  | 4 | `f` | `float` | `float`
| `double` | 8 | `d` | `float` | `double`

## Todo

- [ ] Make C and python functions/objects match ... they are different right now
- [ ] Should I move to a real CRC-8 checksum?
- [ ] Generate C and Python messages from a template instead of writing independently
    - Don't like solution, not sure it is really useful
- [ ] Add method `bool has_valid_chksum()`? Class could calculate checksum and then compare
    to its checksum

### Done

- [x] Remove messages from library. This is just a binary packer and
      not dependant on the message format other than, it is a `struct`
- [x] c++: Make a header only library
- [x] c++: rename `YivoPack_t` to something else like `yivopkt_t`
- [x] c++: Should I move away from `std::vector` and to a fixed array size, since on create of
    a message, I know the complete size ... it isn't a mystery at that point
    - NO
        - would now have to use `new` and `delete` which aren't my favorite
        - `std::array` puts data on stack and can only handle a limited size before
           you overflow the stack

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
