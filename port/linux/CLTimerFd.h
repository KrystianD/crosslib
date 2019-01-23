#ifndef __CLTIMERFD_H__
#define __CLTIMERFD_H__

#include <iostream>
#include <sys/time.h>
#include <stdint.h>
#include <sys/timerfd.h>
#include <unistd.h>
#include <errno.h>

#include <CLCommon.h>
#include <CLEPoll.h>

namespace CROSSLIB_NAMESPACE
{
	class TimerFd
	{
		int tfd;
		timespec ts;

		void createTimer()
		{
			tfd = timerfd_create(CLOCK_MONOTONIC, 0);
		}

	public:
		TimerFd()
		{
			createTimer();
		}
		TimerFd(uint32_t ms)
		{
			createTimer();
			setInterval(ms);
		}
		~TimerFd()
		{
			close(tfd);
		}

		void setInterval(uint32_t ms)
		{
			setInterval(Time::fromMilliseconds(ms));
		}
		void setInterval(const Time& time)
		{
			ts = time.toTimespec();
		}

		bool addToEpoll(EPoll& epoll)
		{
			return epoll.addRead(tfd);
		}

		bool removeFromEpoll(EPoll& epoll)
		{
			return epoll.remove(tfd);
		}

		void start()
		{
			itimerspec spec;
			spec.it_interval = ts;
			spec.it_value = ts;
			timerfd_settime(tfd, 0, &spec, 0);
		}

		void stop()
		{
			itimerspec spec;
			spec.it_interval.tv_sec = 0;
			spec.it_interval.tv_nsec = 0;
			spec.it_value.tv_sec = 0;
			spec.it_value.tv_nsec = 0;
			timerfd_settime(tfd, 0, &spec, 0);
		}

		void get()
		{
			uint64_t val;
			read(tfd, &val, sizeof(uint64_t));
		}

		bool wait()
		{
			get();
			return true;
		}

		int getFd() const { return tfd; }
	};
}

#endif
