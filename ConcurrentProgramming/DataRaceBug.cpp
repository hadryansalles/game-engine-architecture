#include "Tests.h"

#include <atomic>
#include <array>
#include <chrono>
#include <iostream>
#include <mutex>
#include <functional>
#include <thread>
#include <utility>
#include <Windows.h>

#include "../HandlingErrors/Assert.h"

// unnamed namespace 
// visible only within this translation unit
namespace {
	// global data are always initialized with 0
	int g_Count;
	volatile int g_VolatileCount;
	std::atomic<int> g_AtomicCount;
	std::mutex g_Lock;
	HANDLE g_WinLock;
	CRITICAL_SECTION g_CriticalSection;

	struct TestResult {
		int failures;
		long long duration;
	};

	template<typename C, typename F>
	TestResult testIncrementFunction(C&& count, int maxCount, int maxIterations, F&& incrementFunction) {

		auto iterateFunction = [&] {
			for (int i = 0; i < maxCount; i++) {
				incrementFunction();
			}
		};

		int failures = 0;
		auto begin = std::chrono::steady_clock::now();
		for (int i = 0; i < maxIterations; i++) {

			count = 0;

			std::thread threadA(iterateFunction);
			std::thread threadB(iterateFunction);
			std::thread threadC(iterateFunction);

			threadA.join();
			threadB.join();
			threadC.join();

			// if the variable has a different value than the expected
			// increment the number of fails
			failures += count != 3 * maxCount;
		}
		auto end = std::chrono::steady_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
		return TestResult{ failures, duration };
	}
}

void DataRaceBugTest(int maxCount, int maxIterations) {

	std::cout << "\n=========================================================================================================\n";
	std::cout << "\nTesting the chance of data race bug and the time spent for each type of variable.\n\n";

	auto simpleTest = testIncrementFunction(g_Count, maxCount, maxIterations, []() {
		g_Count++;
	});

	auto atomicTest = testIncrementFunction(g_AtomicCount, maxCount, maxIterations, []() {
		g_AtomicCount++;
	});

	auto volatileTest = testIncrementFunction(g_VolatileCount, maxCount, maxIterations, []() {
		g_VolatileCount++;
	});

	auto mutexGuardedTest = testIncrementFunction(g_Count, maxCount, maxIterations, []() {
		{
			std::lock_guard<std::mutex> scopedLock(g_Lock);
			g_Count++;
		}
	});

	g_WinLock = CreateMutex(0, false, 0);
	auto winMutexGuardedTest = testIncrementFunction(g_Count, maxCount, maxIterations, []() {
		WaitForSingleObject(g_WinLock, INFINITE);
		g_Count++;
		ReleaseMutex(g_WinLock);
	});
	if(g_WinLock != NULL) CloseHandle(g_WinLock);
	
	InitializeCriticalSection(&g_CriticalSection);
	auto criticalSectionTest = testIncrementFunction(g_Count, maxCount, maxIterations, []() {
		EnterCriticalSection(&g_CriticalSection);
		g_Count++;
		LeaveCriticalSection(&g_CriticalSection);
	});
	DeleteCriticalSection(&g_CriticalSection);

	std::cout << "Chance | Time (ms) \n";
	std::cout << "Atomic: " << (float) atomicTest.failures / (float) maxIterations << " | " << atomicTest.duration << "\n";
	std::cout << "Volatile: " << (float) volatileTest.failures  / (float) maxIterations << " | " << volatileTest.duration << "\n";
	std::cout << "Simple variable: " << (float) simpleTest.failures  / (float) maxIterations << " | " << simpleTest.duration << "\n";
	std::cout << "Std mutex: " << (float) mutexGuardedTest.failures  / (float) maxIterations << " | " << mutexGuardedTest.duration << "\n";
	std::cout << "Windows mutex: " << (float) winMutexGuardedTest.failures  / (float) maxIterations << " | " << winMutexGuardedTest.duration << "\n";
	std::cout << "Windows critical section: " << (float) criticalSectionTest.failures  / (float) maxIterations << " | " << criticalSectionTest.duration << "\n";
	
	std::cout << "\n=========================================================================================================\n\n";
}
