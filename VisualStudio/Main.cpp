#include <iostream>

void breakWhenCalled() {
	std::cout << "Hello World!\n";
}

void breakWhenCalledWith2(int value) {
	std::cout << "Hello World!\n";
}

void breakAndHelloDebugger() {

}

int main() {
	int watchBinary;
	int breakWhenChanged;
	int watchHex;
	int breakWhenIs4;

	watchBinary = 15; // display 1111 in the watch - works
	breakWhenChanged = 10; // break - works
	watchHex = 15; // display 0xF - works
	breakWhenIs4 = 1; 
	const char* watchOpenThe = "Open the Door!"; // display "Open the" - works
	breakWhenCalledWith2(1); 
	breakWhenChanged = 20; // break - works
	breakWhenIs4 = 2; 
	breakWhenCalled(); // break - works
	breakWhenChanged = 30; // break - works
	breakWhenCalledWith2(2); // break - works
	breakWhenIs4 = 4; // break - works
	breakWhenCalledWith2(3); 
	breakAndHelloDebugger(); // - works
	watchBinary = 63; // display 111111 - works
	watchHex = 127; // display 11111111 - works
	return 0;
}