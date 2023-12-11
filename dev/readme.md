# gentools

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


## ToDo

- [ ] maybe move this to `yivo` since it might be tied to that like `mavlink`
pack/unpack is tied to its messages or `protobuf`
- [x] allow comments on `enum` values
- [x] for complex types, use `&` in args
- [ ] fix python function indent issues
- [ ] enable `yivo` packing and `struct` format
    - `yivo` is a class and an object must exist to use it ... maybe:
        - packing is a function, no state variables needed
        - unpacking is a class for the `parse(byte)`
        - also, don't like `YivoPack_t` ... maybe `ypkt_t`
- [x] handle arrays: `uint8[32]`
