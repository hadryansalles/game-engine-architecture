#pragma once

#include "../HandlingErrors/Assert.h"
#include "ScopedLock.h"

// instead of preventing data-race, it just detects this conditions
class UnnecessaryLock {
public:
	void Acquire() {
		// assert no one has the lock
		ASSERT(!m_Locked);
		// lock	
		m_Locked = true;
	}

	void Release() {
		// assert is currently locked
		ASSERT(m_Locked);
		// unlock
		m_Locked = false;
	}

private:
	volatile bool m_Locked = false;
};

#define ASSERTIONS_ENABLED 1

#if ASSERTIONS_ENABLED
	#define BEGIN_ASSERT_LOCK_NOT_NECESSARY(L) (L).Acquire()
	#define END_ASSERT_LOCK_NOT_NECESSARY(L) (L).Release()
	#define SCOPED_ASSERT_LOCK_NOT_NECESSARY(L, J) ScopedLock<decltype(L)> J(L)
#else
	#define BEGIN_ASSERT_LOCK_NOT_NECESSARY(L) 
	#define END_ASSERT_LOCK_NOT_NECESSARY(L) 
#endif
