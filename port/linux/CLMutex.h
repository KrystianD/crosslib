#ifndef __CLMUTEX_H__
#define __CLMUTEX_H__

#include <pthread.h>
#include <stdint.h>
#include <algorithm>

#include "CLOS.h"

namespace CrossLib {
enum class MutexType { Mutex, RecursiveMutex, Uninitialized };

timespec msToTimeSpec(uint64_t time)
{
	timespec ts;
	ts.tv_sec = time / 1000ul;
	ts.tv_nsec = (time - (ts.tv_sec * 1000ul)) * 1000000ul;
	return ts;
}

class Mutex {
	bool initialized;
	pthread_mutex_t mutex;

public:
	Mutex(MutexType type = MutexType::Mutex) : initialized(true)
	{
		pthread_mutexattr_t attr;
		switch (type) {
		case MutexType::Mutex:
			pthread_mutex_init(&mutex, NULL);
			break;
		case MutexType::RecursiveMutex:
			pthread_mutexattr_init(&attr);
			pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
			pthread_mutex_init(&mutex, &attr);
			pthread_mutexattr_destroy(&attr);
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
		} else {
			uint64_t future = OS::getTime() + timeout;
			timespec timeToWait = msToTimeSpec(future);
			return pthread_mutex_timedlock(&mutex, &timeToWait) == 0;
		}
	}
	bool trylock()
	{
		return pthread_mutex_trylock(&mutex) == 0;
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
	RecursiveMutex() : Mutex(MutexType::RecursiveMutex) { }
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
