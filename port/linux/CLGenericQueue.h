#ifndef __CLGENERICQUEUE_H__
#define __CLGENERICQUEUE_H__

#include <stdint.h>
#include <string.h>

#include "CLMutex.h"
#include "CLCondVar.h"

namespace CROSSLIB_NAMESPACE {
class GenericQueue {
	Mutex mutex;
	CondVar condVarGet, condVarPut;

	uint32_t maxSize, itemSize;
	uint32_t _size, wrIdx, rdIdx;
	uint8_t* array;

public:
	GenericQueue(uint32_t maxSize, uint32_t itemSize)
		: maxSize(maxSize), itemSize(itemSize), _size(0), wrIdx(0), rdIdx(0), array(nullptr)
	{
		array = new uint8_t[maxSize * itemSize];
	}

	~GenericQueue()
	{
		if (array) {
			delete [] array;
			array = nullptr;
		}
	}

	GenericQueue(GenericQueue&& other)
	{
		mutex = std::move(other.mutex);
		condVarGet = std::move(other.condVarGet);
		condVarPut = std::move(other.condVarPut);
		maxSize = other.maxSize;
		_size = other._size;
		rdIdx = other.rdIdx;
		wrIdx = other.wrIdx;
		array = other.array;
		other.array = nullptr;
	}

	GenericQueue& operator=(GenericQueue&& other)
	{
		if (array)
			delete [] array;
		mutex = std::move(other.mutex);
		condVarGet = std::move(other.condVarGet);
		condVarPut = std::move(other.condVarPut);
		maxSize = other.maxSize;
		_size = other._size;
		rdIdx = other.rdIdx;
		wrIdx = other.wrIdx;
		array = other.array;
		other.array = nullptr;
		return *this;
	}

	bool put(const void* item, uint32_t timeout = 0xffffffff)
	{
		MutexGuard guard(mutex);
		if (condVarPut.waitFor(guard, timeout, [this]() { return _size < maxSize; })) {
			memcpy(array + wrIdx * itemSize, item, itemSize);
			_size++;
			wrIdx = (wrIdx + 1) % maxSize;
			condVarGet.notifyOne();
			return true;
		} else {
			return false;
		}
	}

	bool putFront(const void* item, uint32_t timeout = 0xffffffff)
	{
		MutexGuard guard(mutex);
		if (condVarPut.waitFor(guard, timeout, [this]() { return _size < maxSize; })) {
			if (rdIdx == 0)
				rdIdx = maxSize - 1;
			else
				rdIdx--;
			memcpy(array + rdIdx * itemSize, item, itemSize);
			_size++;
			condVarGet.notifyOne();
			return true;
		} else {
			return false;
		}
	}

	bool putFromISR(const void* item, long* xHigherPriorityTaskWoken)
	{
		return put(item, 0);
	}

	bool get(void* item, uint32_t timeout = 0xffffffff)
	{
		MutexGuard guard(mutex);
		if (condVarGet.waitFor(guard, timeout, [this]() { return _size > 0; })) {
			memcpy(item, array + rdIdx * itemSize, itemSize);
			_size--;
			rdIdx = (rdIdx + 1) % maxSize;
			condVarPut.notifyOne();
			return true;
		} else {
			return false;
		}
	}

	bool peek(void* item, uint32_t timeout = 0xffffffff)
	{
		MutexGuard guard(mutex);
		if (condVarGet.waitFor(guard, timeout, [this]() { return _size > 0; })) {
			memcpy(item, array + rdIdx * itemSize, itemSize);
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
	GenericQueue(const GenericQueue&) = delete;
};
}

#endif
