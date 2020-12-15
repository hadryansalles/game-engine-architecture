#pragma once

#include <atomic>

#include "../HandlingErrors/Assert.h"

class ReaderWriterLock {
public:
	bool TryAcquireReader() {
		if (m_RefCount.load(std::memory_order_relaxed) != UINT_MAX) {

			m_RefCount++;

			// acquire fence to ensure all subsequente
			// reads by this thread will be valid
			std::atomic_thread_fence(std::memory_order_acquire);

			return true;
		}
		return false;
	}

	bool TryAcquireWriter() {
		if (m_RefCount.load(std::memory_order_relaxed) == 0) {

			m_RefCount = UINT_MAX;

			// acquire fence to ensure all subsequente
			// reads by this thread will be valid
			std::atomic_thread_fence(std::memory_order_acquire);

			return true;
		}
		return false;
	}

	void ReleaseReader() {
		// release to ensure all subsequent writes
		// have ben committed before we unlock
		std::atomic_thread_fence(std::memory_order_release);

		m_RefCount--;
	}

	void ReleaseWriter() {
		// release to ensure all subsequent writes
		// have ben committed before we unlock
		std::atomic_thread_fence(std::memory_order_release);

		m_RefCount = 0;
	}

	void AcquireReader() {
		while (!TryAcquireReader()) {
			PAUSE();
		}
	}

	void AcquireWriter() {
		while (!TryAcquireWriter()) {
			PAUSE();
		}
	}

private:
	std::atomic<unsigned int> m_RefCount = 0;
};