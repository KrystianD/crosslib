#ifndef __CLOS_H__
#define __CLOS_H__

#include <stdint.h>
#include <stdarg.h>
#include <sys/time.h>

extern "C" {
#include "FreeRTOS.h"
#include "task.h"
}

#include <CLUtils.h>
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
		return Time::fromMilliseconds(xTaskGetTickCount() * 1000 / configTICK_RATE_HZ);
	}
	static void sleep(uint32_t delayMs)
	{
		vTaskDelay(msToTicks(delayMs));
	}
	static void exit(int code)
	{
		for (;;)
			OS::sleep(1000);
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
