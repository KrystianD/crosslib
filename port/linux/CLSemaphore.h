#ifndef __CLSEMAPHORE_H__
#define __CLSEMAPHORE_H__

#include "CLMutex.h"
#include "CLCondVar.h"

namespace CrossLib {
class Semaphore {
	Mutex mutex;
	CondVar cv;
	int count;

public:
	Semaphore() : count(0) { }
	~Semaphore() { }

	void give()
	{
		MutexGuard guard(mutex);
		if (count < 1)
			count++;
		cv.notifyOne();
	}

	bool take(uint32_t timeout = 0xffffffff)
	{
		MutexGuard guard(mutex);
		if (cv.waitFor(guard, timeout, [this]() { return count > 0; })) {
			count--;
			return true;
		} else {
			return false;
		}
	}

private:
	Semaphore(const Semaphore&) = delete;
};
}

#endif
