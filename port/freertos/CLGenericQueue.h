#ifndef __CLGENERICQUEUE_H__
#define __CLGENERICQUEUE_H__

#include <stdint.h>
#include <string.h>

extern "C" {
#include "FreeRTOS.h"
#include "queue.h"
}

namespace crosslib {
class GenericQueue {
	xQueueHandle queue;
	uint32_t itemSize;

public:
	GenericQueue(uint32_t maxSize, uint32_t itemSize)
		: itemSize(itemSize)
	{
		queue = xQueueCreate(maxSize, itemSize);
	}

	~GenericQueue()
	{
		if (queue)
			vQueueDelete(queue);
	}

	GenericQueue(GenericQueue&& other)
	{
		if (queue)
			vQueueDelete(queue);
		queue = other.queue;
		other.queue = nullptr;
	}
	GenericQueue& operator=(GenericQueue&& other)
	{
		if (queue)
			vQueueDelete(queue);
		queue = other.queue;
		other.queue = nullptr;
		return *this;
	}

	bool put(const void* item, uint32_t timeout = 0xffffffff)
	{
		return xQueueSendToBack(queue, item, msToTicks(timeout)) == pdTRUE;
	}

	bool putFromISR(const void* item, long* xHigherPriorityTaskWoken)
	{
		return xQueueSendToBackFromISR(queue, item, xHigherPriorityTaskWoken) == pdTRUE;
	}

	bool get(void* item, uint32_t timeout = 0xffffffff)
	{
		return xQueueReceive(queue, item, msToTicks(timeout)) == pdTRUE;
	}

	bool peek(void* item, uint32_t timeout = 0xffffffff)
	{
		return xQueuePeek(queue, item, msToTicks(timeout)) == pdTRUE;
	}

	uint32_t size()
	{
		return uxQueueMessagesWaiting(queue);
	}

	uint32_t freeSpace()
	{
#ifndef FREERTOS_SIM
		return uxQueueSpacesAvailable(queue);
#else
		return -1;
#endif
	}

	bool clear()
	{
#ifndef FREERTOS_SIM
		return xQueueReset(queue) == pdTRUE;
#else
		return false;
#endif
	}

private:
	GenericQueue(const GenericQueue&) = delete;
};
}

#endif
