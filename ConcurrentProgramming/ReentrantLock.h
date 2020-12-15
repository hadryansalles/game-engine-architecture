#pragma once

#include <atomic>
#include <functional>
#include <thread>

#include "../HandlingErrors/Assert.h"

class ReentrantLock {
public:
	void Acquire() {
		// get thread id
		std::hash<std::thread::id> hasher;
		std::size_t tId = hasher(std::this_thread::get_id());
		
		// if this thread doesn't already hold the lock
		if (m_Atomic.load(std::memory_order_relaxed) != tId) {

			// wait until we hold it
			std::size_t unlockValue = 0;
			while (
				!m_Atomic.compare_exchange_weak (
					unlockValue,
					tId,
					std::memory_order_relaxed,
					std::memory_order_relaxed
				)
			){
				unlockValue = 0;
				PAUSE();
			}
		}
		
		// increase the reentry count
		++m_RefCount;

		// acquire fence to ensure all subsequente
		// reads by this thread will be valid
		std::atomic_thread_fence(std::memory_order_acquire);
	}

	void Release() {
		// release to ensure all subsequent writes
		// have ben committed before we unlock
		std::atomic_thread_fence(std::memory_order_release);
	
		// the thread releasing must be the same holding the lock
		std::hash<std::thread::id> hasher;
		std::size_t actual = m_Atomic.load(std::memory_order_relaxed);
		std::size_t tId = hasher(std::this_thread::get_id());
		ASSERT(tId == actual);
	
		// decrease the reentry count
		--m_RefCount;
		// if reach 0 we unlock it
		if (m_RefCount == 0) {
			m_Atomic.store(0, std::memory_order_relaxed);
		}
	}

	bool TryAcquire() {

		// get thread id
		std::hash<std::thread::id> hasher;
		std::size_t tId = hasher(std::this_thread::get_id());
	
		// check if is already acquired
		bool acquired = false;
		if (tId == m_Atomic.load(std::memory_order_relaxed)) {
			acquired = true;
		} else {
			// if isn't acquired, try to acquire
			std::size_t unlockValue = 0;
			acquired = m_Atomic.compare_exchange_weak(
				unlockValue,
				tId,
				std::memory_order_relaxed,
				std::memory_order_relaxed
			);
		}
		
		if(acquired) {
			// if is already acquired increase the counter
			++m_RefCount;
			// and make sure that subsequent reads are valid
			std::atomic_thread_fence(std::memory_order_acquire);
		}

		return acquired;
	}

private:
	std::atomic<std::size_t> m_Atomic = 0;
	int m_RefCount = 0;
};