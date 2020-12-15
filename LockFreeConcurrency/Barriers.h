#pragma once

#include <atomic>
#include <Windows.h>

#include "../HandlingErrors/Assert.h"

// Only guarateed to not cache its contents in registers
// Doens't necessarily prevent from instruction reordering at compilation
// Some compilers provide this, but not all of them (not required by the std).
// Also don't prevent CPU runtime reordering
volatile int x = 0;

#define COMPILER_BARRIER() _ReadWriteBarrier()

// I couldn't find functions to these instructions...
// Probably don't exists out of atomic variables context
#define MEM_RELEASE_FENCE() STATIC_ASSERT()
#define MEM_ACQUIRE_FENCE() STATIC_ASSERT()

namespace illustrative {

	int g_Ready = 0;
	int g_Data = 0;

	void ProducerThread() {

		g_Data = 42;

		// Don't reorder instructions across this barrier
		COMPILER_BARRIER();
		
		// Read/Write before this barrier CAN'T pass it 
		// MEM_RELEASE_FENCE();

		g_Ready = 1;
	}

	void ConsumerThread() {

		while (!g_Ready) {
			PAUSE();
		}
		
		// Don't reorder instructions across this barrier
		COMPILER_BARRIER();
		
		// Read/Write after this barrier CAN'T pass it 
		// MEM_ACQUIRE_FENCE();

		ASSERT(g_Data == 42);
	}

}

namespace BarriersTest {

	long int g_Produced = 0;
	long int g_Consumed = 0;
	long int g_Failures = 0;

	std::atomic<int> g_Data = 0;
	std::atomic<bool> g_Ready = false;
}

void ProducerThread() {
	
	using namespace BarriersTest;

	// No barrier is used
	g_Data.store(42, std::memory_order_relaxed);
	
	g_Produced++;
	
	// Read/Write before this barrier CAN'T pass it 
	g_Ready.store(true, std::memory_order_release);
}

void ConsumerThread() {
	
	using namespace BarriersTest;
	
	// Read/Write after this barrier CAN'T pass it 
	while (!g_Ready.load(std::memory_order_acquire)) {
		PAUSE();
	}

	// No barrier is used
	g_Failures += g_Data.load(std::memory_order_relaxed) != 42;
	g_Consumed++;
}
