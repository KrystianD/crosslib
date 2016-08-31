#ifndef __CLGENERICQUEUE_H__
#define __CLGENERICQUEUE_H__

#include <stdint.h>

#include <hFramework.h>

namespace CROSSLIB_NAMESPACE {

class GenericQueue {
	hGenericQueue queue;

public:
	GenericQueue(uint32_t maxSize, uint32_t itemSize)
		: queue(maxSize, itemSize)
	{
	}

	~GenericQueue()
	{
	}

	/*GenericQueue(GenericQueue&& other)
	{
		queue = other.queue;
		other.queue = nullptr;
	}
	GenericQueue& operator=(GenericQueue&& other)
	{
		queue = other.queue;
		other.queue = nullptr;
		return *this;
	}*/

	bool put(const void* item, uint32_t timeout = 0xffffffff)
	{
		return queue.put(item, timeout);
	}
	
	bool putFront(const void* item, uint32_t timeout = 0xffffffff)
	{
		return queue.putFront(item, timeout);
	}

	bool putFromISR(const void* item, long* xHigherPriorityTaskWoken)
	{
		return queue.putFromISR(item, xHigherPriorityTaskWoken);
	}

	bool get(void* item, uint32_t timeout = 0xffffffff)
	{
		return queue.get(item, timeout);
	}

	bool peek(void* item, uint32_t timeout = 0xffffffff)
	{
		return queue.peek(item, timeout);
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

private:
	GenericQueue(const GenericQueue&) = delete;
};
}

#endif
