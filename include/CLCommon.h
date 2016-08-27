#ifndef __CLCOMMON_H__
#define __CLCOMMON_H__

#include <stdint.h>
#include <sys/time.h>

namespace crosslib {

struct Time {
	uint64_t _nanoseconds;

	Time& addMS(uint64_t timeMs)
	{
		_nanoseconds += timeMs * 1000000ul;
		return *this;
	}

	uint64_t nanoseconds() const { return _nanoseconds; }
	uint64_t microseconds() const { return _nanoseconds / 1000ul; }
	uint64_t milliseconds() const { return _nanoseconds / 1000000ul; }
	uint64_t seconds() const { return _nanoseconds / 1000000000ul; }

	timespec toTimespec()
	{
		timespec ts;
		ts.tv_sec = _nanoseconds / 1000000000ul;
		ts.tv_nsec = _nanoseconds - ts.tv_sec * 1000000000ul;
		return ts;
	}

	static Time fromSeconds(uint64_t seconds)
	{
		Time time;
		time._nanoseconds = seconds * 1000000000ul;
		return time;
	}

	static Time fromMilliseconds(uint64_t milliseconds)
	{
		Time time;
		time._nanoseconds = milliseconds * 1000000ul;
		return time;
	}

	static Time fromMicroseconds(uint64_t microseconds)
	{
		Time time;
		time._nanoseconds = microseconds * 1000ul;
		return time;
	}

	static Time fromTimeval(timeval& tv)
	{
		Time time;
		time._nanoseconds = tv.tv_sec * 1000000000ul + tv.tv_usec * 1000;;
		return time;
	}

	static Time fromTimespec(timespec& tv)
	{
		Time time;
		time._nanoseconds = tv.tv_sec * 1000000000ul + tv.tv_nsec;
		return time;
	}

	friend Time operator-(const Time& a, const Time& b)
	{
		Time time;
		time._nanoseconds = a._nanoseconds - b._nanoseconds;
		return time;
	}
};

}

#endif
