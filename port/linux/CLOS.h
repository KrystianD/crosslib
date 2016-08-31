#ifndef __CLOS_H__
#define __CLOS_H__

#include <stdint.h>
#include <stdarg.h>
#include <sys/time.h>
#include <unistd.h>

#include <CLCommon.h>

extern void crosslib_on_error(const char* fmt, va_list arg);

namespace CROSSLIB_NAMESPACE {

class OS {
public:
	static uint64_t getTimeMS()
	{
		return getTime().milliseconds();
	}
	static Time getTime()
	{
		struct timeval now;
		gettimeofday(&now, nullptr);
		return Time::fromTimeval(now);
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
	static Time getClockRealtime()
	{
		struct timespec ts;
		clock_gettime(CLOCK_REALTIME, &ts);
		return Time::fromTimespec(ts);
	}
	static Time getClockMonotonic()
	{
		struct timespec ts;
		clock_gettime(CLOCK_MONOTONIC, &ts);
		return Time::fromTimespec(ts);
	}
};
}

#endif
