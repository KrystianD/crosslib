#ifndef __CLOS_H__
#define __CLOS_H__

#include <stdint.h>

#include <CLUtils.h>

#include <hFramework.h>

namespace crosslib {
class OS {
public:
	static uint32_t getTime32()
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
};
}

#endif
