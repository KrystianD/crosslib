#include <sys/time.h>
#include <functional>

#include "CLSemaphore.h"

extern "C" {
#include <FreeRTOS.h>
#include <task.h>
}

namespace CROSSLIB_NAMESPACE
{
	typedef std::function<void(void*)> HandlerUserData;
	typedef std::function<void()> Handler;

	struct ThreadAttributes
	{
		int stackSize, priority;
		const char* name;

		ThreadAttributes() : stackSize(configMINIMAL_STACK_SIZE * 4), priority(-1), name("unnamed") {}

		void setPriority(int priority)
		{
			this->priority = priority;
		}
	};

	class Thread
	{
	public:
		Thread();
		Thread(const Handler& handler);
		Thread(const HandlerUserData& handler, void* userdata);
		Thread(const ThreadAttributes& attrs, const Handler& handler);
		Thread(const ThreadAttributes& attrs, const HandlerUserData& handler, void* userdata);
		~Thread();

		Thread(Thread&& other)
		{
			if (taskHandle)
				OS::error("cannot move to object with started thread");
			if (other.taskHandle)
				OS::error("cannot move started thread");
			handler = std::move(other.handler);
			taskSemaphore = std::move(other.taskSemaphore);
			attrs = other.attrs;
		}
		Thread& operator=(Thread&& other)
		{
			if (taskHandle)
				OS::error("cannot move to object with started thread");
			if (other.taskHandle)
				OS::error("cannot move started thread");
			handler = std::move(other.handler);
			taskSemaphore = std::move(other.taskSemaphore);
			attrs = other.attrs;
			return *this;
		}

		void start();
		bool join(uint32_t timeout = 0xffffffff);
		bool hasStarted() const;
		bool isRunning() const;

		xTaskHandle getHandle() const { return taskHandle; }

	private:
		ThreadAttributes attrs;
		Handler handler;

		xTaskHandle taskHandle;
		Semaphore taskSemaphore;
		bool running;

		Thread(const Thread&) = delete;

		friend void threadFunc(void* arg);
	};
}
