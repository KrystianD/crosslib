#ifndef __CLQUEUE_H__
#define __CLQUEUE_H__

#include <stdint.h>
#include <string.h>

#include "CLMutex.h"
#include "CLCondVar.h"

namespace CrossLib {
template<typename T>
class Queue {
public:
	Queue(uint32_t maxSize)
		: maxSize(maxSize), _size(0), array(0), wrIdx(0), rdIdx(0)
	{
		array = new uint8_t[maxSize * sizeof(T)];
	}

	~Queue()
	{
		if (array)
			delete [] array;
	}

	bool put(const T& item, uint32_t timeout = 0xffffffff)
	{
		MutexGuard guard(mutex);
		if (condVarPut.waitFor(guard, timeout, [this]() { return _size < maxSize; })) {
			memcpy(array + wrIdx * sizeof(T), &item, sizeof(T));
			_size++;
			wrIdx = (wrIdx + 1) % maxSize;
			condVarGet.notifyOne();
			return true;
		} else {
			return false;
		}
	}

	bool get(T& item, uint32_t timeout = 0xffffffff)
	{
		MutexGuard guard(mutex);
		if (condVarGet.waitFor(guard, timeout, [this]() { return _size > 0; })) {
			memcpy(&item, array + rdIdx * sizeof(T), sizeof(T));
			_size--;
			rdIdx = (rdIdx + 1) % maxSize;
			condVarPut.notifyOne();
			return true;
		} else {
			return false;
		}
	}

	bool peek(T& item, uint32_t timeout = 0xffffffff)
	{
		MutexGuard guard(mutex);
		if (condVarGet.waitFor(guard, timeout, [this]() { return _size > 0; })) {
			memcpy(&item, array + rdIdx * sizeof(T), sizeof(T));
			return true;
		} else {
			return false;
		}
	}

	uint32_t size() const
	{
		MutexGuard guard(mutex);
		return _size;
	}

	bool clear()
	{
		MutexGuard guard(mutex);
		wrIdx = rdIdx = _size = 0;
		condVarPut.notifyOne();
		return true;
	}

private:
	Mutex mutex;
	CondVar condVarGet, condVarPut;

	uint32_t maxSize;
	uint32_t _size, wrIdx, rdIdx;
	uint8_t* array;

	Queue(const Queue&) = delete;
};


}

#endif
