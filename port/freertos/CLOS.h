#ifndef __CLOS_H__
#define __CLOS_H__

#include <stdint.h>
#include <sys/time.h>

extern "C" {
#include "FreeRTOS.h"
#include "task.h"
// #include "queue.h"
// #include "croutine.h"
}

namespace crosslib {
class OS {
public:
	static uint32_t getTime32()
	{
		return xTaskGetTickCount() * 1000 / configTICK_RATE_HZ;
	}
	static uint64_t getTime()
	{
		return xTaskGetTickCount() * 1000 / configTICK_RATE_HZ;
	}
};
}

#endif
