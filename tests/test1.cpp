#include <crosslib.h>
#include <gtest/gtest.h>

using namespace crosslib;

volatile int cnt = 0;
volatile int cnt1 = 0;
volatile int cnt2 = 0;
volatile bool ok;

void crosslib_on_error(const char* fmt, va_list arg)
{
	vfprintf(stderr, fmt, arg);
	va_end(arg);
}

void fn0()
{
	cnt++;
}
TEST(crosslib, thread)
{
	Thread th(fn0);
	th.run();
	th.join();
	ASSERT_EQ(cnt, 1);
}

void fn1()
{
	OS::sleep(30);
}
TEST(crosslib, thread_join)
{
	Thread th(fn1);
	th.run();
	ASSERT_EQ(th.join(0), false);
	ASSERT_EQ(th.join(10), false);
	ASSERT_EQ(th.join(50), true);
}

void fn2()
{
	OS::sleep(30);
}
TEST(crosslib, thread_isrunning)
{
	Thread th(fn2);
	th.run();
	ASSERT_EQ(th.isRunning(), 1);
	OS::sleep(50);
	ASSERT_EQ(th.isRunning(), 0);
}

TEST(crosslib, thread_stack)
{
	ThreadAttributes attrs;
	attrs.stackSize = 20000;
	Thread th(attrs, fn0);
	th.run();
	th.join();
}

void fn3()
{
	for (int i = 0; i < 10; i++) {
		cnt1++;
		for (volatile int i = 0; i < 1000000; i++) ;
	}
}
void fn4()
{
	for (int i = 0; i < 10; i++) {
		cnt2++;
		for (volatile int i = 0; i < 1000000; i++) ;
	}
}
TEST(crosslib, thread_same_priority)
{
	ThreadAttributes attrs;

	attrs.setPriority(Scheduler::RoundRobin, 2);
	Thread th1(attrs, fn3);

	attrs.setPriority(Scheduler::RoundRobin, 2);
	Thread th2(attrs, fn4);

	cnt1 = cnt2 = 0;
	th1.run();
	th2.run();

	th1.join();
	ASSERT_EQ(cnt1, 10);
	ASSERT_EQ(cnt2, 10);
	th2.join();
	ASSERT_EQ(cnt1, 10);
	ASSERT_EQ(cnt2, 10);
}

TEST(crosslib, thread_different_priority)
{
	ThreadAttributes attrs;

	attrs.setPriority(Scheduler::RoundRobin, 2);
	Thread th1(attrs, fn3);

	attrs.setPriority(Scheduler::RoundRobin, 1);
	Thread th2(attrs, fn4);

	cnt1 = cnt2 = 0;
	th1.run();
	th2.run();

	th1.join();
	ASSERT_EQ(cnt1, 10);
	ASSERT_EQ(cnt2, 0);
	th2.join();
	ASSERT_EQ(cnt1, 10);
	ASSERT_EQ(cnt2, 10);
}

TEST(crosslib, thread_move_ctr)
{
	Thread th(fn0);
	Thread th2(std::move(th));
	th2.run();
	th2.join();
}

Thread th_t;
TEST(crosslib, thread_move_ctr2)
{
	th_t = Thread(fn0);
}

TEST(crosslib, thread_move_assign)
{
	Thread th(fn0);
	Thread th2;
	th2 = std::move(th);
	th2.run();
	th2.join();
}

void fn5(void* ptr)
{
	ok = ptr == (void*)0xaabbccdd;
}
TEST(crosslib, thread_userdata)
{
	void* userdata = (void*)0xaabbccdd;
	Thread th(fn5, userdata);
	ok = false;
	th.run();
	th.join();
	ASSERT_EQ(ok, true);
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
