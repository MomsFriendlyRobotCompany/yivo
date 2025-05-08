# `yivo-gen`

A simple message builder for C++ and Python.

## Why?

This is based off of the `protobuf` message format, but with 
some differences:

- Removed `repeated` and all arrays required to have sizes defined
  - Why? This way all message sizes are fixed and
  pre-calculated
  - Also replaced `string` with `char[size]`
- Added `type` which are data types, but not `messages` so
you cannot send them by themselves because they don't have
a message id
  - Why? Makes it easier to declare new data types. 
  Normal data types are in package `std` (like C++),
  such as `uint32`, `int8`, `float`, `double`, etc.
    - Yes, replaced `float32` with `float` and 
    `float64` with `double`. Seemed unnecessary to do it
    the `protobuf` way

## Message Format

```
// comment
package name;

import "package/name"

define NAME = value; // c: #define NAME (value) // comment

constant type NAME = number;
constant char NAME[] = string; // size automatically calculated

// enums will all be sized to int8 -128 to 127
enum NAME {
  // comment
  a = 0; // comment
  b = 1;
}

// will be stored in package variable table
// so message sizes can be calculated
constexpr uint32_t type_name_variable_array_size = number;
constexpr uint32_t type_name_size = number;
type NAME {
  type variable;
  type variable[array_size];
  type variable = number;
  type variable = number; // comment
  int32 variable = 1_000_000; // use python numbers
  float variable = 1e-3;
  double variable = -23.012;
}

message NAME: ID {
  same as type
}
```

## Process

```
file -> dict -> FormaterC -> file
```

## todo

[ ] union like: `oneof contact { float x,y,z; float v[3]; }`
  - maybe too much work?