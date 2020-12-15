#include <iostream>
#include <atomic>
#include <windows.h>

#define MAX_THREADS 3

DWORD WINAPI functionA(LPVOID lpParam) {
	std::cout << "Hello World from A\n";

	// 2 - termination
	ExitThread(0);

	DebugBreak();
}

DWORD WINAPI functionB(LPVOID lpParam) {
	std::cout << "Hello World from B\n";

	for (;;) {
		std::cout << "Waiting for someone to kill me...\n";
		Sleep(500); // sleep for 0.5 seconds
	}

	return 0;
}

DWORD WINAPI functionC(LPVOID lpParam) {
	std::cout << "Hello World from C\n";

	std::atomic_flag* exit = (std::atomic_flag*)lpParam;
	while (exit->test_and_set()) {
		// 5 - yield
		SwitchToThread();
	}

	return 0;
}

int main() {

	// 1 - creation
	DWORD ids[MAX_THREADS];
	HANDLE threads[MAX_THREADS];

	std::atomic_flag exit = ATOMIC_FLAG_INIT;
	exit.test_and_set();

	threads[0] = CreateThread(NULL, 0, functionA, 0, 0, &ids[0]);
	threads[1] = CreateThread(NULL, 0, functionB, 0, 0, &ids[1]);
	threads[2] = CreateThread(NULL, 0, functionC, (LPVOID)&exit, 0, &ids[2]);

	if (!threads[0] || !threads[1] || !threads[2]) {
		ExitProcess(1);
	}
	
	// 4 - sleep
	Sleep(3000); // sleep for 3 seconds
	
	exit.clear();
	
	// 3 - request to exit
	TerminateThread(threads[1], 0);

	// 6 - join
	WaitForMultipleObjects(MAX_THREADS, threads, true, INFINITE);
	
	for (int i = 0; i < MAX_THREADS; i++) {
		CloseHandle(threads[i]);
	}

	return 0;
}