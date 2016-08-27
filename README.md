# crosslib
Cross OS library with support for Linux and FreeRTOS


Usage
======

```cmake
set(CROSSLIB_PORT "linux")
include(<PATH_TO_CROSSLIB>/crosslib.cmake)

add_executable(program main.cpp ${COMMON_SOURCES})
```
