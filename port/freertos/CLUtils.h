#ifndef __CLUTILS_H__
#define __CLUTILS_H__

#include <stdint.h>

extern "C" {
#include "FreeRTOS.h"
}

namespace crosslib {

static inline uint32_t msToTicks(uint32_t ms)
{
	if (ms == 0xffffffff) {
		return 0xffffffff;
	} else {
		return ms * configTICK_RATE_HZ / 1000;
	}
}

}

#endif
