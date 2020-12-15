#pragma once

#include <atomic>

#include "../HandlingErrors/Assert.h"

class SpinLock {
public:
	bool TryAcquire() {
		// use acquire fence to ensure all subsequent 
		// reads by this thread will be valid
		bool alreadyLocked = m_Atomic.test_and_set(std::memory_order_acquire);
		return !alreadyLocked;
	}
	
	void Acquire() {
		while (!TryAcquire()) {
			// give a hint to the processor that 
			// the code is a busy-wait loop
			PAUSE();
		}
	}

	void Release() {
		// use release fence to ensure all prior
		// writes have been commited before we unlock 
		m_Atomic.clear(std::memory_order_release);
	}

private:
	std::atomic_flag m_Atomic = ATOMIC_FLAG_INIT;
};
