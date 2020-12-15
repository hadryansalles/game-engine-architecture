#include "Tests.h"

int main() {

	ReaderWriterLockTest();
	UnnecessaryLockTest();
	ReentrantLockTest();
	ProducerConsumerTest();
	DataRaceBugTest(1500, 150);

	return 0;
}
