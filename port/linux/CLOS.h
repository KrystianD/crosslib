#ifndef __CLOS_H__
#define __CLOS_H__

#include <stdint.h>
#include <sys/time.h>

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
};
}

#endif
