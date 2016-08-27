#ifndef __CLMUTEX_H__
#define __CLMUTEX_H__

#include <pthread.h>
#include <stdint.h>
#include <algorithm>

#include "CLOS.h"
#include "CLUtils.h"

namespace crosslib {
enum class MutexType { Normal, Recursive, Uninitialized };

class Mutex {
	bool initialized;
	pthread_mutex_t mutex;

public:
	Mutex(MutexType type = MutexType::Normal) : initialized(true)
	{
		pthread_mutexattr_t attr;
		switch (type) {
		case MutexType::Normal:
			pthread_mutex_init(&mutex, NULL);
			break;
		case MutexType::Recursive:
			pthread_mutexattr_init(&attr);
			pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
			pthread_mutex_init(&mutex, &attr);
			pthread_mutexattr_destroy(&attr);
			break;
		default:
			break;
		}
	}

	virtual ~Mutex()
	{
		if (!initialized)
			return;
		pthread_mutex_destroy(&mutex);
	}

	Mutex(Mutex&& other)
	{
		initialized = other.initialized;
		mutex = other.mutex;
		other.initialized = false;
	}
	Mutex& operator=(Mutex&& other)
	{
		if (initialized)
			pthread_mutex_destroy(&mutex);
		initialized = other.initialized;
		mutex = other.mutex;
		other.initialized = false;
		return *this;
	}

	bool unlock()
	{
		return pthread_mutex_unlock(&mutex) == 0;
	}

	bool lock(uint32_t timeout = 0xffffffff)
	{
		if (timeout == 0xffffffff) {
			return pthread_mutex_lock(&mutex) == 0;
		} else if (timeout == 0) {
			return pthread_mutex_trylock(&mutex) == 0;
		} else {
			uint64_t future = OS::getTime() + timeout;
			timespec timeToWait = msToTimeSpec(future);
			return pthread_mutex_timedlock(&mutex, &timeToWait) == 0;
		}
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
