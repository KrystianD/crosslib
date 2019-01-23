#ifndef __CLCONDVAR_H__
#define __CLCONDVAR_H__

#include <pthread.h>
#include <sys/time.h>
#include <functional>

#include "CLMutex.h"
#include "CLSemaphore.h"

namespace CROSSLIB_NAMESPACE
{
	class CondVar
	{
		Semaphore s, x;
		volatile bool isWaiting;

	public:
		CondVar()
		{
			x.give();
			s.take(0);
		}

		~CondVar()
		{
		}

		// only one thread at a time can call this
		void notifyOne()
		{
			x.take();
			if (isWaiting)
				s.give();
			x.give();
		}

		// only one thread at a time can call this
		bool wait(Mutex& mutex, uint32_t timeout = 0xffffffff)
		{
			x.take();
			isWaiting = true;
			x.give();

			mutex.unlock();

			bool res = s.take(timeout);

			mutex.lock();

			return res;
		}

		// only one thread at a time can call this
		bool waitFor(MutexGuard& guard, std::function<bool()> func)
		{
			return waitFor(guard, 0xffffffff, func);
		}

		// only one thread at a time can call this
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
