![](https://raw.githubusercontent.com/MomsFriendlyRobotCompany/yivo/main/docs/yivo.png)

# Yivo

**Work in Progress**

# API

| Part | In Checksum | Type  | Description |
|------|-------------|-------|-------------|
| Header |   | `uint8_t[2]`  | default `$K` |
| Size   | x | `uint16_t`    | 0-65,535 bytes |
| Type   | x | `uint8_t`     | 256 message IDs |
| Data   | x | `uint8_t[NN]` | payload data array |
| Checksum | | `uint8_t`     | XOR of size, type, and data bytes |

| 0 | 1 | 2 | 3 | 4 | ... | -1 |
|---|---|---|---|---|-----|----|
|`$`|`K`| N | N | T | ... | checksum |

## Todo

- [ ] Should I move to a 16b checksum? I don't have many messages, but
      the payload can be 16b long
- [x] Remove messages from library. This is just a binary packer and
      not dependant on the message format other than, it is a `struct`
- [x] Make a header only library

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
