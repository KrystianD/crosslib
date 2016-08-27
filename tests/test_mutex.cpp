#include <crosslib.h>
#include <gtest/gtest.h>

using namespace crosslib;

void crosslib_on_error(const char* fmt, va_list arg)
{
	vfprintf(stderr, fmt, arg);
	va_end(arg);
}

TEST(crosslib, mutex_test)
{
	Mutex mutex;

	mutex.lock();

	Thread th([&mutex]() { OS::sleep(100); mutex.unlock(); });
	th.start();

	ASSERT_EQ(mutex.lock(20), false);
	ASSERT_EQ(mutex.lock(300), true);

	th.join();
}

int main(int argc, char **argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
