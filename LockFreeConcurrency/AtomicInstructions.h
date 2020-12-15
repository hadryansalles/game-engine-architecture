#pragma once

#include <Windows.h>

#include "../HandlingErrors/Assert.h"

// Other simpler atomic instructions: Read, Write, Read-Modify-Write 

// Test and Set instruction 
#ifdef _WIN64
	#define TAS(p) _interlockedbittestandset64((LONG64*)p, 0)
#elif _WIN32
	#define TAS(p) _interlockedbittestandset((LONG*)p, 0)
#else
	#define TAS(p)
#endif

// Exchange instruction for 8 bit words
#define XCHG8(p, v) _InterlockedExchange8((CHAR*)p, (CHAR)v)

// Compare and Swap instruction 
#ifdef _WIN64
	#define CAS(p, e, v) _InterlockedCompareExchange64((LONG64*)p, (LONG64)v, (LONG64)e)
#elif _WIN32
	#define CAS(p, e, v) _InterlockedCompareExchange((LONG*)p, (LONG)v, (LONG)e)
#else
	#define TAS(p) STATIC_ASSERT(false)
#endif

// I couldn't find Visual Studio functions for emitting these instructions...
#define LL(p) false
#define SC(p) false

// illustrative examples, lack proper memory fencing.
namespace illustrative {
	/*
	 * Test and Set
	 * Pseudocode for TAS instruction
	 * Set the variabl to 1 and return its previous value
	 */
	bool TestAndSet(bool* pLock) {
		const bool old = *pLock;
		*pLock = true;
		return old;
	}
	
	/*
	 * Exchange 
	 * Pseudocode for Exchange instruction
	 * Exchange the values of two variables
	 */
	void Exchange(bool* pv1, bool* pv2) {
		bool temp = *pv1;
		*pv1 = *pv2;
		*pv2 = temp;
	}

	/*
	 * Compare and Swap
	 * Pseudocode for CAS instruction
	 * Checks the existing value and waps it with a new value if,
	 * and only if the existing value matches an expected value
	 */
	bool CompareAndSwap(int* pValue, int expectedValue, int newValue) {
		// atomically	
		if (*pValue == expectedValue) {
			*pValue = newValue;
			return true;
		}
		return false;
	}

	// CAS can be breaked into LL and SC
	
	namespace LLSC {

		int* linkRegister = nullptr;

		/*
		 * Load Linked
		 * Reads the memory atomically and store its address in the link register
		 */
		int LoadLinked(int* pValue) {
			linkRegister = pValue;
			return *pValue;
		}

		/*
		 * Store Conditional
		 * Writes the value into the given address, 
		 * but only if the address matches the one stored in the link register
		 */
		bool StoreConditional(int* pValue, int newValue) {
			if (linkRegister = pValue) {
				*pValue = newValue;
				return true;
			}
			return false;
		}
	}

	// TAS can be used to implement a Spin Lock TAS
	void SpinLockTAS(bool* pLock) {
		// check if is currently set up
		while(TAS(pLock) == true) {
			PAUSE(); // while someone else has lock... we wait...
		}
	}
	
	// SpinLock implemented using Exchange instruction
	void SpinLockXCHG(bool* pLock) {
		bool old = true;
		while (true) {
			XCHG8(pLock, old);
			if (!old) {
				// we succesfully locked
				break;
			}
			PAUSE();
		}
	}

	// Spinlock implemented using CAS instruction
	void SpinLockCAS(int* pValue) {
		const int kLockValue = -1;
		// while the existing value is different than 0
		while (!CAS(pValue, 0, kLockValue)) {
			PAUSE(); // is locked by someone else
		}
	}

	void AtomicIncrementCAS(int* pValue) {
		while (true) {
			const int oldValue = *pValue;
			const int newValue = oldValue + 1;
			if (CAS(pValue, oldValue, newValue)) {
				break;
			}
			PAUSE();
		}
	}

	void AtomicIncrementLLSC(int* pValue) {
		while (true) {
			const int oldValue = LL(*pValue);
			const int newValue = oldValue + 1;
			if (SC(pValue, newValue)) {
				break;
			}
			PAUSE();
		}
	}
}
