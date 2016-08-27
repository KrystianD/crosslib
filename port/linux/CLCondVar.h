#ifndef __CLCONDVAR_H__
#define __CLCONDVAR_H__

#include <pthread.h>
#include <sys/time.h>
#include <functional>

#include "CLMutex.h"

namespace crosslib {
class CondVar {
	bool initialized;
	pthread_cond_t cond;

public:
	CondVar()
	{
		pthread_condattr_t attr;
		pthread_condattr_init(&attr);
		if (pthread_condattr_setclock(&attr, CLOCK_MONOTONIC) != 0)
			OS::error("unable to set clock to MONOTONIC");

		pthread_cond_init(&cond, &attr);
		pthread_condattr_destroy(&attr);

		initialized = true;
	}

	~CondVar()
	{
		if (!initialized)
			return;
		pthread_cond_destroy(&cond);
	}

	CondVar(CondVar&& other)
	{
		initialized = other.initialized;
		cond = other.cond;
		other.initialized = false;
	}
	CondVar& operator=(CondVar&& other)
	{
		if (initialized)
			pthread_cond_destroy(&cond);

		initialized = other.initialized;
		cond = other.cond;
		other.initialized = false;
		return *this;
	}

	void notifyOne()
	{
		pthread_cond_signal(&cond);
	}
	void notifyAll()
	{
		pthread_cond_broadcast(&cond);
	}
	bool wait(Mutex& mutex, uint32_t timeout = 0xffffffff)
	{
		pthread_mutex_t *pmutex = (pthread_mutex_t*)mutex.getMutex();
		if (timeout == 0xffffffff) {
			return pthread_cond_wait(&cond, pmutex) == 0;
		} else {
			Time future = OS::getClockMonotonic().addMS(timeout);
			timespec ts = future.toTimespec();
			return pthread_cond_timedwait(&cond, pmutex, &ts) == 0;
		}
	}
	bool waitFor(MutexGuard& guard, std::function<bool()> func)
	{
		return waitFor(guard, 0xffffffff, func);
	}
	bool waitFor(MutexGuard& guard, uint32_t timeout, std::function<bool()> func)
	{
		Mutex& mutex = guard.getMutex();
		uint64_t startTime = OS::getTimeMS();

		while (!func()) {
			uint32_t toWait;
			if (timeout == 0xffffffff) {
				toWait = 0xffffffff;
			} else {
				uint64_t elapsed = OS::getTimeMS() - startTime;
				if (elapsed >= timeout)
					return false;
				toWait = (uint32_t)(timeout - elapsed);
			}
			wait(mutex, toWait);
		}
		return true;
	}
};
}

#endif
