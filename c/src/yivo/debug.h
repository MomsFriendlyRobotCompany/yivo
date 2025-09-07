#pragma once

#if 0
  #define YIVO_DEBUG(format, ...)                                              \
    do {                                                                       \
      printf(format, ##__VA_ARGS__);                                           \
    } while (0)
#else
  #define YIVO_DEBUG(format, ...)
#endif