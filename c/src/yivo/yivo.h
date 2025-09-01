/******************************************************************************
MIT License

Copyright (c) 2020 Mom's Friendly Robot Company

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
******************************************************************************/
#pragma once

#ifndef __YIVO_HPP__
  #define __YIVO_HPP__
#endif

#define YIVO_HEADER_0 '$'
#define YIVO_HEADER_1 'K'

#define YIVO_PARSER_NULL -1 // parser or payload
#define YIVO_PKT_NULL -2    // pkt or data
#define YIVO_SRC_NULL -3
#define YIVO_SIZE_ERROR -4
#define YIVO_PKT_HEADER_ERR -5
#define YIVO_PKT_ID_ERR -6
#define YIVO_PKT_CS_ERR -7

#include <stdbool.h>
#include <stdint.h>

#define YMSG __attribute__((packed))

#include "yivo/parser.h"
#include "yivo/yivopkt.h"
