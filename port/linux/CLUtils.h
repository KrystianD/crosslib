#ifndef __CLUTILS_H__
#define __CLUTILS_H__

#include <stdint.h>
#include <time.h>

namespace crosslib {

static inline timespec msToTimeSpec(uint64_t time)
{
	timespec ts;
	ts.tv_sec = time / 1000ul;
	ts.tv_nsec = (time - (ts.tv_sec * 1000ul)) * 1000000ul;
	return ts;
}
static inline uint64_t timeSpecToNs(const timespec& ts)
{
	return ts.tv_sec * 1000000000 + ts.tv_nsec;
}

}

#endif
