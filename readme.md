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
    - C
    - Python

## API

| Part     | Type            | Description                                       |
|----------|-----------------|---------------------------------------------------|
| Header   | `uint8_t[2]`    | default `$K`                                      |
| Size     | `uint16_t`      | 0-65,535 bytes, stored as [L,H] => `(H << 8) | L` |
| Type     | `uint8_t`       | 255 message IDs, `0` is not allowed as an ID      |
| Data     | `uint8_t[Size]` | payload data array                                |
| Checksum | `uint8_t`       | XOR of N Data bytes                               |

| 0 | 1 | 2 | 3 | 4 | 5  | 0 - N|
|---|---|---|---|---|----|------|
|`$`|`K`| L | H | T | CS | Data |

## Byte Conversions

| Type     | Bytes | Format | Python  | C/C++         |
|----------|-------|--------|---------|---------------|
| `uint8`  | 1     | `B`    | `int`   | `uint8_t`
| `uint16` | 2     | `H`    | `int`   | `uint16_t`
| `uint32` | 4     | `I`    | `int`   | `uint32_t`
| `uint64` | 8     | `Q`    | `int`   | `uint64_t`
| `int8`   | 1     | `b`    | `int`   | `int8_t`
| `int16`  | 2     | `h`    | `int`   | `int16_t`
| `int32`  | 4     | `i`    | `int`   | `int32_t`
| `int64`  | 8     | `q`    | `int`   | `int64_t`
| `float`  | 4     | `f`    | `float` | `float`
| `double` | 8     | `d`    | `float` | `double`

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
