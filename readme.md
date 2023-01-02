![](https://raw.githubusercontent.com/MomsFriendlyRobotCompany/yivo/main/docs/yivo.png)

# Yivo

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
CS: checksum
```

```python
from yivo import Yivo

some_data = range(9)

yivo = Yivo()
msg = rr.pack(MsgIDs.RAW_IMU, some_data) # create binary message
# ... send over serial link and receive at other end ...
err, msg_id, data = rr.unpack(msg) # unpack msg
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
