#ifndef __CLMUTEX_H__
#define __CLMUTEX_H__

#include <stdint.h>
#include <algorithm>

#include "CLOS.h"
#include "CLUtils.h"

#include <hFramework.h>

namespace CROSSLIB_NAMESPACE {

class Mutex : public hMutex {
public:
	Mutex() { }
private:
	Mutex(const Mutex&) = delete;
};

class RecursiveMutex : public hRecursiveMutex {
public:
	RecursiveMutex() { }
private:
	RecursiveMutex(const RecursiveMutex&) = delete;
};

class MutexGuard {
public:
	MutexGuard(Mutex& mutex) : mutex(mutex) { mutex.lock(); }
	~MutexGuard() { mutex.unlock(); }

	Mutex& getMutex() { return mutex; }

private:
	Mutex& mutex;

	MutexGuard(const MutexGuard&) = delete;
};

}

#endif
