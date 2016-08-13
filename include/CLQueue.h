#ifndef __CLQUEUE_H__
#define __CLQUEUE_H__

#include <stdint.h>
#include <string.h>

namespace CrossLib {
template<typename T>
class Queue {
	GenericQueue queue;

public:
	Queue(uint32_t maxSize) : queue(maxSize, sizeof(T))
	{
	}

	~Queue()
	{
	}

	Queue(Queue&& other)
	{
		queue = std::move(other.queue);
	}
	Queue& operator=(Queue&& other)
	{
		queue = std::move(other.queue);
		return *this;
	}

	bool put(const T& item, uint32_t timeout = 0xffffffff)
	{
		return queue.put(&item, timeout);
	}

	bool putFromISR(const T& item, long* xHigherPriorityTaskWoken)
	{
		return queue.put(&item, xHigherPriorityTaskWoken);
	}

	bool get(T& item, uint32_t timeout = 0xffffffff)
	{
		return queue.get(&item, timeout);
	}

	bool peek(T& item, uint32_t timeout = 0xffffffff)
	{
		return queue.peek(&item, timeout);
	}

	uint32_t size()
	{
		return queue.size();
	}

	uint32_t freeSpace()
	{
		return queue.freeSpace();
	}

	bool clear()
	{
		return queue.clear();
	}

	// misc
	int putMultiple(T items[], int length, uint32_t timeout = 0xffffffff)
	{
		for (int i = 0; i < length; i++) {
			T& item = items[i];
			if (out(item, timeout))
				timeout = 0;
			else
				return i;
		}
		return length;
	}

	int putMultipleAll(T items[], int length, uint32_t timeout = 0xffffffff)
	{
		int pos = 0;
		uint64_t startTime = OS::getTime();
		while (pos < length) {
			uint32_t toWait;
			if (timeout == 0xffffffff) {
				toWait = 0xffffffff;
			} else {
				uint64_t elapsed = OS::getTime() - startTime;
				if (elapsed >= timeout)
					break;
				toWait = (uint32_t)(timeout - elapsed);
			}

			int r = putMultiple(items + pos, length - pos, toWait);
			pos += r;
		}
		return pos;
	}

	int getMultiple(T items[], int length, uint32_t timeout = 0xffffffff)
	{
		for (int i = 0; i < length; i++) {
			T& item = items[i];
			if (get(item, timeout))
				timeout = 0;
			else
				return i;
		}
		return length;
	}

	int getMultipleAll(T items[], int length, uint32_t timeout = 0xffffffff)
	{
		int pos = 0;
		uint64_t startTime = OS::getTime();
		while (pos < length) {
			uint32_t toWait;
			if (timeout == 0xffffffff) {
				toWait = 0xffffffff;
			} else {
				uint64_t elapsed = OS::getTime() - startTime;
				if (elapsed >= timeout)
					break;
				toWait = (uint32_t)(timeout - elapsed);
			}

			int r = getMultiple(items + pos, length - pos, toWait);
			pos += r;
		}
		return pos;
	}

private:
	Queue(const Queue&) = delete;
};
}

#endif
