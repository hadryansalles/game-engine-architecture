#include <atomic>
#include <chrono>
#include <iostream>
#include <thread>
#include <random>

#include "Barriers.h"
#include "LinkedList.h"

int main() {
	
	std::cout << "\nTesting how many productions/consumptions this approach can get in 1 seconds.\n\n";
	
	std::atomic<bool> run = true;

	std::thread producer([&]() {
		while (run) {
			ProducerThread();
		}
	});

	std::thread consumer([&]() {
		while (run) {
			ConsumerThread();
		}
	});

	std::this_thread::sleep_for(std::chrono::seconds(1));

	run = false;
	producer.join();
	consumer.join();

	std::cout << "Productions | Consumptions | Failures (data ins't produced when consumed)\n";
	std::cout << "Not safe: " << BarriersTest::g_Produced << " | " << BarriersTest::g_Consumed << " | " << BarriersTest::g_Failures << "\n";

	std::cout << "\nTesting thread-safe lock-free linked list\n\n";

	std::random_device rd;
	std::mt19937 mt(rd());
	std::uniform_real_distribution<float> dist(0.0, 1.0);

	LockFreeList<float> list;
	constexpr int maxCount = 100;

	auto genList = [&]() {
		for (int i = 0; i < maxCount; i++) {
			list.PushFront(dist(mt));
		}
	};

	std::thread listThreadA(genList);
	std::thread listThreadB(genList);

	listThreadA.join();
	listThreadB.join();

	std::cout << "Number of elements: Expected: " << 2 * maxCount << " Get: " << list.CountNodes() << "\n";

	return 0;
}