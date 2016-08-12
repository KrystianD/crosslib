#ifndef __TCCONDVAR_H__
#define __TCCONDVAR_H__

#include <pthread.h>
#include <sys/time.h>
#include <functional>

#include "CLMutex.h"

namespace CrossLib {
class CondVar {
	pthread_cond_t cond;

public:
	CondVar()
	{
		pthread_cond_init(&cond, NULL);
	}

	~CondVar()
	{
		pthread_cond_destroy(&cond);
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
		if (timeout == 0xffffffff) {
			return pthread_cond_wait(&cond, (pthread_mutex_t*)mutex.getMutex()) == 0;
		} else {
			uint64_t future = OS::getTime() + timeout;
			timespec timeToWait = msToTimeSpec(future);
			return pthread_cond_timedwait(&cond, (pthread_mutex_t*)mutex.getMutex(), &timeToWait) == 0;
		}
	}
	bool waitFor(MutexGuard& guard, std::function<bool()> func)
	{
		return waitFor(guard, 0xffffffff, func);
	}
	bool waitFor(MutexGuard& guard, uint32_t timeout, std::function<bool()> func)
	{
		pthread_mutex_t *pmutex = (pthread_mutex_t*)guard.getMutex().getMutex();

		uint64_t startTime = OS::getTime();

		for (;;) {
			if (func())
				return true;

			if (timeout == 0xffffffff) {
				pthread_cond_wait(&cond, pmutex);
			} else {
				uint64_t elapsed = OS::getTime() - startTime;
				if (elapsed >= timeout)
					return false;
				uint64_t toWait = timeout - elapsed;

				uint64_t future = OS::getTime() + toWait;
				timespec timeToWait = msToTimeSpec(future);
				pthread_cond_timedwait(&cond, pmutex, &timeToWait);
			}
		}
	}
};
}

#endif
