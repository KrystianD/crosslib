#include <crosslib.h>
#include <gtest/gtest.h>

using namespace crosslib;

void crosslib_on_error(const char* fmt, va_list arg)
{
	vfprintf(stderr, fmt, arg);
	va_end(arg);
}

TEST(crosslib, utils_time)
{
	Time time = Time::fromSeconds(1);

	ASSERT_EQ(time.nanoseconds(), 1000000000);

	time.addMS(10);
	ASSERT_EQ(time.nanoseconds(), 1010000000);

	time.addMS(2000);
	ASSERT_EQ(time.nanoseconds(), 3010000000);

	Time d = time - Time::fromMilliseconds(500);
	ASSERT_EQ(d.nanoseconds(), 2510000000);

	Time d2 = d - Time::fromMicroseconds(500);
	ASSERT_EQ(d2.nanoseconds(), 2509500000);
}

int main(int argc, char **argv)
{
	cpu_set_t set;
	CPU_ZERO(&set);
	CPU_SET(2, &set);
	sched_setaffinity(0, sizeof(set), &set);

	Thread::currentSetScheduler(Scheduler::RoundRobin, 10);

	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
