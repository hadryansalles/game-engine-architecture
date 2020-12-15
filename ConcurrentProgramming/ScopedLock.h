#pragma once

#include "SpinLock.h"

template<typename T>
class ScopedLock {
public:

	ScopedLock(T& lock)
		: m_Lock(lock) {
		m_Lock.Acquire();
	}
	
	~ScopedLock() {
		m_Lock.Release();
	}

	ScopedLock(ScopedLock<T>&&) = delete;
	ScopedLock(const ScopedLock<T>&) = delete;
	ScopedLock<T>& operator=(ScopedLock<T>&&) = delete;
	ScopedLock<T>& operator=(const ScopedLock<T>&) = delete;

private:
	T& m_Lock;
};