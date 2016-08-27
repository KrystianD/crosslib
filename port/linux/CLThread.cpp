#include "CLThread.h"

#include <stdio.h>
#include <limits.h>

#include "CLUtils.h"
#include "CLOS.h"

namespace crosslib {

void* threadFunc(void* arg)
{
	Thread* thread = (Thread*)arg;
	thread->handler();
	return 0;
}

static inline int schedToVal(Scheduler sched)
{
	switch (sched) {
	case Scheduler::Default:
	case Scheduler::Other: return SCHED_OTHER;
	case Scheduler::Batch: return  SCHED_BATCH;
	case Scheduler::Idle: return  SCHED_IDLE;
	case Scheduler::FIFO: return  SCHED_FIFO;
	case Scheduler::RoundRobin: return  SCHED_RR;
	}
}

Thread::Thread()
	: thread(0)
{
}
Thread::Thread(const Handler& handler)
	: handler(handler), thread(0)
{
}
Thread::Thread(const HandlerUserData& handler, void* userdata)
	: handler([this,handler,userdata]() { handler(userdata); }), thread(0)
{
}
Thread::Thread(const ThreadAttributes& attrs, const Handler& handler)
	: attrs(attrs), handler(handler), thread(0)
{
}
Thread::Thread(const ThreadAttributes& attrs, const HandlerUserData& handler, void* userdata)
	: attrs(attrs), handler([this,handler,userdata]() { handler(userdata); }), thread(0)
{
}

Thread::~Thread()
{
	if (isRunning()) {
		OS::error("thread is running while destroying an object (id: %lu, name: %s)", thread, attrs.name);
	}
}

void Thread::run()
{
	if (!handler)
		OS::error("no handler for thread");

	int stackSize = attrs.stackSize;
	if (stackSize < PTHREAD_STACK_MIN)
		stackSize = PTHREAD_STACK_MIN;

	Scheduler sched = attrs.scheduler;

	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	pthread_attr_setstacksize(&attr, stackSize);

	if (sched != Scheduler::Default) {
		pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
		pthread_attr_setschedpolicy(&attr, schedToVal(attrs.scheduler));
	}

	if (attrs.priority != -1) {
		sched_param sp;
		sp.sched_priority = attrs.priority;

		if (sched != Scheduler::RoundRobin && sched != Scheduler::FIFO) {
			OS::error("scheduler for priority must be RouindRobin or FIFO");
		}

		pthread_attr_setschedparam(&attr, &sp);
	}

	pthread_create(&thread, &attr, threadFunc, this);
	pthread_setname_np(thread, attrs.name);
	pthread_attr_destroy(&attr);
}
bool Thread::join(uint32_t timeout)
{
	if (timeout == 0xffffffff) {
		return pthread_join(thread, 0) == 0;
	} else if (timeout == 0) {
		return pthread_tryjoin_np(thread, 0) == 0;
	} else {
		uint64_t future = OS::getTime() + timeout;
		timespec timeToWait = msToTimeSpec(future);
		return pthread_timedjoin_np(thread, 0, &timeToWait) == 0;
	}
}

bool Thread::hasStarted() const
{
	return thread != 0;
}
bool Thread::isRunning() const
{
	if (thread == 0) // thread has not been started
		return false;

	int res = pthread_kill(thread, 0);
	if (res == 0) { // thread is running
		return true;
	} else if (res == ESRCH) { // thread is not running
		return false;
	} else {
		OS::error("invalid kill call (id: %lu, name: %s)", thread, attrs.name);
	}
}

void Thread::currentSetScheduler(Scheduler sched, int priority)
{
	int val = schedToVal(sched);
	sched_param sp;
	sp.sched_priority = priority;
	pthread_setschedparam(pthread_self(), val, &sp);
}

}
