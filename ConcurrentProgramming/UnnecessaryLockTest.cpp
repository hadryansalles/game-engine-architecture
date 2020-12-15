#include "Tests.h"

#include <thread>

#include "UnnecessaryLock.h"

void UnnecessaryLockTest() {

	std::cout << "\n=========================================================================================================\n\n";
	std::cout << "\nTesting unnecessary lock.\n\n";

	UnnecessaryLock coutLock;

	std::thread a([&]() {
		BEGIN_ASSERT_LOCK_NOT_NECESSARY(coutLock);
		std::cout << "Hi from thread A\n";
		END_ASSERT_LOCK_NOT_NECESSARY(coutLock);
	});
	
	a.join();

	std::thread b([&]() {
		SCOPED_ASSERT_LOCK_NOT_NECESSARY(coutLock, coutGuard);
		std::cout << "Hi from thread B\n";
	});

	b.join();

	std::cout << "\n=========================================================================================================\n\n";
}