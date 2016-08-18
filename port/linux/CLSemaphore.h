#ifndef __CLSEMAPHORE_H__
#define __CLSEMAPHORE_H__

#include "CLMutex.h"
#include "CLCondVar.h"

namespace crosslib {
class Semaphore {
	Mutex mutex;
	CondVar cv;
	int count;

public:
	Semaphore() : count(0) { }
	~Semaphore() { }

	Semaphore(Semaphore&& other)
	{
		mutex = std::move(other.mutex);
		cv = std::move(other.cv);
		count = other.count;
	}
	Semaphore& operator=(Semaphore&& other)
	{
		mutex = std::move(other.mutex);
		cv = std::move(other.cv);
		count = other.count;
		return *this;
	}

	bool give()
	{
		MutexGuard guard(mutex);
		if (count < 1)
			count++;
		cv.notifyOne();
		return true;
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

	bool trytake()
	{
		return take(0);
	}

private:
	Semaphore(const Semaphore&) = delete;
};
}

#endif
