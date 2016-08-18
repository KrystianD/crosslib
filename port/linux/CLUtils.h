#ifndef __CLUTILS_H__
#define __CLUTILS_H__

#include <stdint.h>

namespace crosslib {

static inline timespec msToTimeSpec(uint64_t time)
{
	timespec ts;
	ts.tv_sec = time / 1000ul;
	ts.tv_nsec = (time - (ts.tv_sec * 1000ul)) * 1000000ul;
	return ts;
}

}

#endif
