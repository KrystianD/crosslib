#ifndef __CLMUTEX_H__
#define __CLMUTEX_H__

#include <stdint.h>
#include <algorithm>

extern "C" {
#include "FreeRTOS.h"
#include "semphr.h"
}

#include "CLOS.h"
#include "CLUtils.h"

namespace crosslib {
enum class MutexType { Normal, Recursive, Uninitialized };

class Mutex {
	MutexType type;
	xSemaphoreHandle mutex;

public:
	Mutex(MutexType type = MutexType::Mutex) : type(type)
	{
		switch (type) {
		case MutexType::Normal:
			mutex = xSemaphoreCreateMutex();
			break;
		case MutexType::Recursive:
			mutex = xSemaphoreCreateRecursiveMutex();
			break;
		}
	}

	virtual ~Mutex()
	{
#ifndef FREERTOS_SIM
		if (type != MutexType::Uninitialized)
			vSemaphoreDelete(mutex);
#endif
	}

	Mutex(Mutex&& other)
	{
		type = other.type;
		mutex = other.mutex;
		other.type = MutexType::Uninitialized;
	}
	Mutex& operator=(Mutex&& other)
	{
#ifndef FREERTOS_SIM
		if (type != MutexType::Uninitialized)
			vSemaphoreDelete(mutex);
#endif
		type = other.type;
		mutex = other.mutex;
		other.type = MutexType::Uninitialized;
		return *this;
	}

	bool unlock()
	{
		if (type == MutexType::Recursive)
			return xSemaphoreGiveRecursive(mutex) == pdTRUE;
		else
			return xSemaphoreGive(mutex) == pdTRUE;
	}

	bool lock(uint32_t timeout = 0xffffffff)
	{
		if (type == MutexType::Recursive)
			return xSemaphoreTakeRecursive(mutex, msToTicks(timeout)) == pdTRUE;
		else
			return xSemaphoreTake(mutex, msToTicks(timeout)) == pdTRUE;
	}

	bool trylock()
	{
		return lock(0);
	}

	void* getMutex()
	{
		return &mutex;
	}

private:
	Mutex(const Mutex&) = delete;
};

class RecursiveMutex : public Mutex {
public:
	RecursiveMutex() : Mutex(MutexType::Recursive) { }
	~RecursiveMutex() { }
};

class MutexGuard {
public:
	MutexGuard(Mutex& mutex) : mutex(mutex) { mutex.lock(); }
	~MutexGuard() { mutex.unlock(); }

	Mutex& getMutex() { return mutex; }

private:
	Mutex& mutex;

	MutexGuard(const MutexGuard&) = delete;
};

}

#endif
