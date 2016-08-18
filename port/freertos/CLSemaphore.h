#ifndef __CLSEMAPHORE_H__
#define __CLSEMAPHORE_H__

#include <stdint.h>
#include <algorithm>

extern "C" {
#include "FreeRTOS.h"
#include "semphr.h"
}

#include "CLOS.h"
#include "CLUtils.h"

namespace crosslib {

class Semaphore {
	xSemaphoreHandle semphr;

public:
	Semaphore()
	{
		vSemaphoreCreateBinary(semphr);
	}

	~Semaphore()
	{
#ifndef FREERTOS_SIM
		if (semphr)
			vSemaphoreDelete(semphr);
#endif
	}

	Semaphore(Semaphore&& other)
	{
		semphr = other.semphr;
		other.semphr = 0;
	}
	Semaphore& operator=(Semaphore&& other)
	{
#ifndef FREERTOS_SIM
		if (semphr)
			vSemaphoreDelete(semphr);
#endif
		semphr = other.semphr;
		other.semphr = 0;
		return *this;
	}

	bool give()
	{
		return xSemaphoreGive(semphr) == pdTRUE;
	}

	bool take(uint32_t timeout = 0xffffffff)
	{
		return xSemaphoreTake(semphr, msToTicks(timeout)) == pdTRUE;
	}

	bool trytake()
	{
		return take(0);
	}

private:
	Semaphore(const Semaphore&) = delete;
};

}

#endif
