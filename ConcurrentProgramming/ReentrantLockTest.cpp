#include "Tests.h"

#include <iostream>

#include "SpinLock.h"
#include "ReentrantLock.h"
#include "ScopedLock.h"

void ReentrantLockTest() {

	// reentrant lock test
	std::cout << "\n=========================================================================================================\n\n";
	std::cout << "\nTesting reentrant lock.\n\n";

	ReentrantLock coutLock;

	auto A = [&]() {
		ScopedLock<decltype(coutLock)> lock(coutLock);
		std::cout << "Inside A holding cout's lock.\n";
	};

	auto B = [&]() {
		ScopedLock<decltype(coutLock)> lock(coutLock);
		A();
		std::cout << "Inside B holding cout's lock.\n";
	};

	std::thread ta(B);
	std::thread tb(B);

	ta.join();
	tb.join();
	
	std::cout << "\n=========================================================================================================\n\n";
}