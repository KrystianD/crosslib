#ifndef __CLOS_H__
#define __CLOS_H__

#include <stdint.h>
#include <stdarg.h>
#include <sys/time.h>
#include <unistd.h>

extern void crosslib_on_error(const char* fmt, va_list arg);

namespace crosslib {
class OS {
public:
	static uint32_t getTime32()
	{
		struct timeval now;
		gettimeofday(&now, nullptr);
		return now.tv_sec * 1000ul + now.tv_usec / 1000ul;
	}
	static uint64_t getTime()
	{
		struct timeval now;
		gettimeofday(&now, nullptr);
		return now.tv_sec * 1000ul + now.tv_usec / 1000ul;
	}
	static void sleep(uint32_t delayMs)
	{
		usleep(delayMs * 1000);
	}
	static void exit(int code)
	{
		exit(code);
	}
	static void error(const char* fmt, ...)
	{
		va_list arg;
		va_start(arg, fmt);
		crosslib_on_error(fmt, arg);
		exit(99);
	}
};
}

#endif
