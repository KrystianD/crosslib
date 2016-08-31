#ifndef __CLOS_H__
#define __CLOS_H__

#include <stdint.h>

#include <CLUtils.h>

#include <hFramework.h>

namespace CROSSLIB_NAMESPACE {

class OS {
public:
	static uint64_t getTimeMS()
	{
		return sys.getRefTime();
	}
	static uint64_t getTime()
	{
		return sys.getRefTime();
	}
	static void sleep(uint32_t delayMs)
	{
		sys.delay(delayMs);
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
		sys.vlog(fmt, arg);
		exit(99);
	}
};
}

#endif
