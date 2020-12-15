#include <iostream>
#include "Assert.h"

int main() {
	ASSERT(true);
	ASSERT(false);
	SLOW_ASSERT(true);
	SLOW_ASSERT(false);
	STATIC_ASSERT(true);
	// STATIC_ASSERT(false);
	T_STATIC_ASSERT(true);
	// T_STATIC_ASSERT(false);
	return 0;
}