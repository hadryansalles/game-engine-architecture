#include "Tests.h"

#include <atomic>
#include <chrono>
#include <functional>
#include <mutex>
#include <thread>
#include <Windows.h>

#include "../HandlingErrors/Assert.h"
#include "Semaphore.h"
#include "ScopedLock.h"

namespace NotSafe {
	
	// global variables are always initialized with 0, but... for the sake of redundancy.
	long int g_Produced = 0;
	long int g_Consumed = 0;
	long int g_Failures = 0;
	int g_Data;
	bool g_Ready = false;
	std::atomic<bool> g_Open;

	void Producer() {
		while (g_Open.load()) {
			g_Produced++;
			g_Data = 1;
			g_Ready = true;
		}
	}

	void Consumer() {
		while (g_Open.load()) {
			// this is a busy-wait loop...
			// wasting a lot of CPU cycles...
			while (g_Open.load() && !g_Ready) {}
			
			g_Failures += g_Data != 1;
			g_Consumed++;
			g_Ready = false;
			g_Data = -1;
		}
	}
}

namespace LockGuarded {
	
	// global variables are always initialized with 0, but... for the sake of redundancy.
	long int g_Produced = 0;
	long int g_Consumed = 0;
	long int g_Failures = 0;
	int g_Data;
	std::mutex g_Mutex;
	bool g_Ready = false;
	std::atomic<bool> g_Open;
;
	void Producer() {
		while (g_Open.load()) {
			{
				std::unique_lock<std::mutex> lock(g_Mutex);
				g_Produced++;
				g_Data = 1;
				g_Ready = true;
			}

			std::this_thread::yield();
		}
	}

	void Consumer() {
		while (g_Open.load()) {
			// this is a busy-wait loop...
			// wasting a lot of CPU cycles...
			while (true) {

				bool ready = false;
				{
					std::unique_lock<std::mutex> lock(g_Mutex);
					ready = g_Ready;
				}

				if (ready) {
					break;
				}
			}

			{
				std::unique_lock<std::mutex> lock(g_Mutex);
				g_Failures += g_Data != 1;
				g_Consumed++;
				g_Data = -1;
				g_Ready = false;
			}

			std::this_thread::yield();
		}
	}
}

namespace WithConditionVariable {

	// global variables are always initialized with 0, but... for the sake of redundancy.
	long int g_Produced = 0;
	long int g_Consumed = 0;
	long int g_Failures = 0;
	int g_Data;
	std::mutex g_Mutex;
	bool g_Ready = false;
	std::atomic<bool> g_Open;
	std::condition_variable g_CV;

	void Producer() {
		while (g_Open.load()) {
			std::unique_lock<std::mutex> lock(g_Mutex);
			g_Produced++;
			g_Data = 1;
			g_Ready = true;
			g_CV.notify_all();
		}
	}

	void Consumer() {
		while (g_Open.load()) {
		
			std::unique_lock<std::mutex> lock(g_Mutex);
			
			while (!g_Ready) {
				// wait on condition variable, releasing the lock 
				g_CV.wait(lock); 
			}
			
			g_Failures += g_Data != 1;
			g_Consumed++;
			g_Data = -1;
			g_Ready = false;
		}
	}
}

namespace WithSemaphore {

	// global variables are always initialized with 0, but... for the sake of redundancy.
	long int g_Produced = 0;
	long int g_Consumed = 0;
	long int g_Failures = 0;
	int g_Data;
	std::atomic<bool> g_Open;
	Semaphore g_Used(0);
	Semaphore g_Free(1);

	void Producer() {
		while (g_Open.load()) {
			// produce the data locally
			const int produced = 1;
			
			g_Free.Take();
			{
				g_Data = produced;
				g_Produced++;
			}
			g_Used.Give();
		}
	}

	void Consumer() {
		while (g_Open.load()) {
		
			int data = 0;
		
			g_Used.Take();
			{
				data = g_Data;
				g_Data = -1;
				g_Consumed++;
			}
			g_Free.Give();

			// consume the data locally	
			g_Failures += data != 1;
		}
	}
}

namespace WindowsEvents {

	// global variables are always initialized with 0, but... for the sake of redundancy.
	long int g_Produced = 0;
	long int g_Consumed = 0;
	long int g_Failures = 0;
	int g_Data;
	std::atomic<bool> g_Open;
	HANDLE g_Used;
	HANDLE g_Free;

	void Producer() {
		while (g_Open.load()) {
			// produce the data locally
			const int produced = 1;
			
			WaitForSingleObject(g_Free, INFINITE);
			{
				g_Data = produced;
				g_Produced++;
			}
			SetEvent(g_Used);
		}
	}

	void Consumer() {
		while (g_Open.load()) {
		
			int data = 0;
		
			WaitForSingleObject(g_Used, INFINITE);
			{
				data = g_Data;
				g_Data = -1;
				g_Consumed++;
			}
			SetEvent(g_Free);

			// consume the data locally	
			g_Failures += data != 1;
		}
	}
}

namespace MySpinLock {

	// global variables are always initialized with 0, but... for the sake of redundancy.
	long int g_Produced = 0;
	long int g_Consumed = 0;
	long int g_Failures = 0;
	int g_Data;
	SpinLock m_Lock;
	bool g_Ready = false;
	std::atomic<bool> g_Open;
;
	void Producer() {
		while (g_Open.load()) {
			{
				ScopedLock<decltype(m_Lock)> lockGuard(m_Lock);
				g_Produced++;
				g_Data = 1;
				g_Ready = true;
			}
			
			std::this_thread::yield();
		}
	}

	void Consumer() {
		while (g_Open.load()) {

			while (true) {
				// this is a busy-wait loop...
				// wasting a lot of CPU cycles...
				bool ready = false;
				{
					ScopedLock<decltype(m_Lock)> lockGuard(m_Lock);
					ready = g_Ready;
				}

				if (ready) {
					break;
				}
			}
		
			{
				ScopedLock<decltype(m_Lock)> lockGuard(m_Lock);
				g_Failures += g_Data != 1;
				g_Data = -1;
				g_Consumed++;
				g_Ready = false;
			}

			std::this_thread::yield();
		}
	}
}

void RunProducerConsumerExample(std::atomic<bool>& open, std::function<void()> producer, std::function<void()> consumer) {

	open.store(true);

	std::thread producerThread(producer);
	std::thread consumerThread(consumer);

	std::this_thread::sleep_for(std::chrono::seconds(1));
	open.store(false);

	producerThread.join();
	consumerThread.join();
	
}

void ProducerConsumerTest() {
	std::cout << "=========================================================================================================\n";
	std::cout << "\nTesting how many productions/consumptions each approach can get in 1 second.\n\n";

	RunProducerConsumerExample (
		NotSafe::g_Open,
		NotSafe::Producer,
		NotSafe::Consumer
	);

	RunProducerConsumerExample (
		LockGuarded::g_Open,
		LockGuarded::Producer,
		LockGuarded::Consumer
	);
	
	RunProducerConsumerExample (
		WithConditionVariable::g_Open,
		WithConditionVariable::Producer,
		WithConditionVariable::Consumer
	);

	RunProducerConsumerExample (
		WithSemaphore::g_Open,
		WithSemaphore::Producer,
		WithSemaphore::Consumer
	);

	WindowsEvents::g_Used = CreateEvent(nullptr, false, false, nullptr);
	WindowsEvents::g_Free = CreateEvent(nullptr, false, true,  nullptr);
	
	RunProducerConsumerExample (
		WindowsEvents::g_Open,
		WindowsEvents::Producer,
		WindowsEvents::Consumer
	);
	
	if (WindowsEvents::g_Used) {
		CloseHandle(WindowsEvents::g_Used);
	}
	if (WindowsEvents::g_Free) {
		CloseHandle(WindowsEvents::g_Free);
	}

	RunProducerConsumerExample(
		MySpinLock::g_Open,
		MySpinLock::Producer,
		MySpinLock::Consumer
	);

	std::cout << "Productions | Consumptions | Failures (data ins't produced when consumed)\n";
	std::cout << "Not safe: " << NotSafe::g_Produced << " | " << NotSafe::g_Consumed << " | " << NotSafe::g_Failures << "\n";
	std::cout << "Simple lock with busy-wait loop: " << LockGuarded::g_Produced << " | " << LockGuarded::g_Consumed << " | " << LockGuarded::g_Failures << "\n";
	std::cout << "Lock with condition variable: " << WithConditionVariable::g_Produced << " | " << WithConditionVariable::g_Consumed << " | " << WithConditionVariable::g_Failures << "\n";
	std::cout << "Two semaphores: " << WithSemaphore::g_Produced << " | " << WithSemaphore::g_Consumed << " | " << WithSemaphore::g_Failures << "\n";
	std::cout << "Windows Events: " << WindowsEvents::g_Produced << " | " << WindowsEvents::g_Consumed << " | " << WindowsEvents::g_Failures << "\n";
	std::cout << "My Spin Lock: " << MySpinLock::g_Produced << " | " << MySpinLock::g_Consumed << " | " << MySpinLock::g_Failures << "\n";
	std::cout << "\n=========================================================================================================\n\n";
}
