#include "Tests.h"

#include <thread>
#include <random>
#include <vector>

#include "ReaderWriterLock.h"

void ReaderWriterLockTest() {
	
	std::cout << "\n=========================================================================================================\n\n";
	std::cout << "\nTesting reader writer lock.\n\n";
	
	ReaderWriterLock lock;
	int data = 0;

	auto reader = [&]() {
		lock.AcquireReader();
		std::cout << data;
		lock.ReleaseReader();
	};

	auto writer = [&]() {
		lock.AcquireWriter();
		data++;
		lock.ReleaseWriter();
	};

	std::random_device rd;
	std::mt19937 mt(rd());
	std::uniform_int<int> dist(0, 1);
	
	std::vector<std::thread> threads;
	for (int i = 0; i < 50; i++) {
		switch(dist(mt)) {
			case 0:
				threads.push_back(std::thread(writer));
				break;

			case 1:
				threads.push_back(std::thread(reader));
				break;

			default:
				ASSERT(false);
				break;
		}
	}

	for (auto& thread : threads) {
		thread.join();
	}

	std::cout << "\n=========================================================================================================\n\n";
}