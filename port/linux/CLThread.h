#ifndef __CLTHREAD_H__
#define __CLTHREAD_H__

#include <pthread.h>
#include <sys/time.h>
#include <functional>
#include <signal.h>

#include "CLOS.h"

namespace CROSSLIB_NAMESPACE
{
	typedef std::function<void(void*)> HandlerUserData;
	typedef std::function<void()> Handler;

	enum class Scheduler
	{
		Default, Other, Batch, Idle, FIFO, RoundRobin
	};

	struct ThreadAttributes
	{
		int stackSize, priority;
		Scheduler scheduler;
		const char* name;

		ThreadAttributes()
						: stackSize(2 * 1024 * 1024), priority(-1),
						  scheduler(Scheduler::Default), name("unnamed")
		{
		}

		void setPriority(Scheduler scheduler, int priority)
		{
			this->scheduler = scheduler;
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
			if (thread)
				OS::error("cannot move to object with started thread");
			if (other.thread)
				OS::error("cannot move started thread");

			handler = std::move(other.handler);
			attrs = other.attrs;
		}
		Thread& operator=(Thread&& other)
		{
			if (thread)
				OS::error("cannot move to object with started thread");
			if (other.thread)
				OS::error("cannot move started thread");

			handler = std::move(other.handler);
			attrs = other.attrs;
			return *this;
		}

		void start();
		bool join(uint32_t timeout = 0xffffffff);
		bool hasStarted() const;
		bool isRunning() const;

		int getId();
		pthread_t getHandle() const { return thread; }

		static void currentSetScheduler(Scheduler sched, int priority);

	private:
		ThreadAttributes attrs;
		Handler handler;

		pthread_t thread;

		Thread(const Thread&) = delete;

		friend void* threadFunc(void* arg);
	};
}

#endif
