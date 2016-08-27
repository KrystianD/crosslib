#include <crosslib.h>
#include <gtest/gtest.h>

using namespace crosslib;

void crosslib_on_error(const char* fmt, va_list arg)
{
	vfprintf(stderr, fmt, arg);
	va_end(arg);
}

TEST(crosslib, mutex_condvar)
{
	Mutex mutex;
	CondVar cv;

	Thread th([&cv]() { OS::sleep(100); cv.notifyOne(); });
	th.start();

	mutex.lock();
	ASSERT_EQ(cv.wait(mutex, 50), false);
	ASSERT_EQ(cv.wait(mutex, 150), true);

	th.join();
}

int main(int argc, char **argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
