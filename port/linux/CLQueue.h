#ifndef __CLQUEUE_H__
#define __CLQUEUE_H__

#include <stdint.h>
#include <string.h>

#include "CLMutex.h"
#include "CLCondVar.h"

namespace CrossLib {
template<typename T>
class Queue {
	Mutex mutex;
	CondVar condVarGet, condVarPut;

	uint32_t maxSize;
	uint32_t _size, wrIdx, rdIdx;
	uint8_t* array;

public:
	Queue(uint32_t maxSize)
		: maxSize(maxSize), _size(0), array(0), wrIdx(0), rdIdx(0)
	{
		array = new uint8_t[maxSize * sizeof(T)];
	}

	~Queue()
	{
		if (array) {
			delete [] array;
			array = 0;
		}
	}

	Queue(Queue&& other)
	{
		mutex = std::move(other.mutex);
		condVarGet = std::move(other.condVarGet);
		condVarPut = std::move(other.condVarPut);
		maxSize = other.maxSize;
		_size = other._size;
		wrIdx = other.wrIdx;
		array = other.array;
		other.array = nullptr;
	}
	Queue& operator=(Queue&& other)
	{
		if (array)
			delete [] array;
		mutex = std::move(other.mutex);
		condVarGet = std::move(other.condVarGet);
		condVarPut = std::move(other.condVarPut);
		maxSize = other.maxSize;
		_size = other._size;
		wrIdx = other.wrIdx;
		array = other.array;
		other.array = nullptr;
		return *this;
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

	uint32_t size()
	{
		MutexGuard guard(mutex);
		return _size;
	}

	uint32_t freeSpace()
	{
		MutexGuard guard(mutex);
		return maxSize - _size;
	}

	bool clear()
	{
		MutexGuard guard(mutex);
		wrIdx = rdIdx = _size = 0;
		condVarPut.notifyOne();
		return true;
	}

private:
	Queue(const Queue&) = delete;
};


}

#endif
