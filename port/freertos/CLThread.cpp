#include "CLThread.h"

#include <stdio.h>

namespace CROSSLIB_NAMESPACE
{
	void threadFunc(void* arg)
	{
		Thread* thread = (Thread*)arg;

		thread->handler();

		thread->taskSemaphore.give();
		thread->running = false;
		vTaskDelete(NULL);
	}

	Thread::Thread()
					: taskHandle(0)
	{
	}
	Thread::Thread(const Handler& handler)
					: handler(handler), taskHandle(0)
	{
	}
	Thread::Thread(const HandlerUserData& handler, void* userdata)
					: handler([this, handler, userdata]() { handler(userdata); }), taskHandle(0)
	{
	}
	Thread::Thread(const ThreadAttributes& attrs, const Handler& handler)
					: attrs(attrs), handler(handler), taskHandle(0)
	{
	}
	Thread::Thread(const ThreadAttributes& attrs, const HandlerUserData& handler, void* userdata)
					: attrs(attrs), handler([this, handler, userdata]() { handler(userdata); }), taskHandle(0)
	{
	}
	Thread::~Thread()
	{
		if (isRunning()) {
			OS::error("thread is running while destroying an object (name: %s)", attrs.name);
		}
	}

	void Thread::start()
	{
		if (!handler)
			OS::error("no handler for thread");

		int prio = 1;
		if (attrs.priority != -1)
			prio = attrs.priority;

		running = true;
		if (xTaskCreate(&threadFunc,
		                (const char*)attrs.name,
		                attrs.stackSize / sizeof(uint32_t),
		                this,
		                prio,
		                &taskHandle) != pdTRUE) {
			OS::error("unable to create thread");
		}
		taskSemaphore.take(0);
	}
	bool Thread::join(uint32_t timeout)
	{
		return taskSemaphore.take(timeout);
	}

	bool Thread::hasStarted() const
	{
		return taskHandle != 0;
	}
	bool Thread::isRunning() const
	{
		if (taskHandle != 0) {
			// eTaskState state = eTaskGetState(taskHandle);
			// return state != eDeleted;
			return running;
		} else {
			return false;
		}
	}
}
