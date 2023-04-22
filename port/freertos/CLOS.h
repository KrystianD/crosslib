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

extern void CROSSLIB_LOGFUNCNAME(const char* fmt, va_list arg);

namespace CROSSLIB_NAMESPACE
{
	class OS
	{
	public:
		static uint32_t getTimeMs()
		{
			return xTaskGetTickCount() * 1000 / configTICK_RATE_HZ;
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
			CROSSLIB_LOGFUNCNAME(fmt, arg);
			exit(99);
		}
	};
}

#endif
