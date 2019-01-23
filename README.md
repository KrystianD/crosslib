# crosslib
Cross OS library with support for Linux and FreeRTOS


Usage
======

```cmake
set(CROSSLIB_PORT "linux")
include(<PATH_TO_CROSSLIB>/crosslib.cmake)

add_executable(program main.cpp ${COMMON_SOURCES})
```

Error handling
======

```c++
#include <stdio.h>
#include <stdarg.h>

void crosslib_on_error(const char* fmt, va_list arg)
{
	fprintf(stderr, "CrossLib error: ");
	vfprintf(stderr, fmt, arg);
}
```
