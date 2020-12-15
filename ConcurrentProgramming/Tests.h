#pragma once

#include "../HandlingErrors/Assert.h"

/*
 * Runs a loop incrementing different types of variables simultaneously in three threads.
 * After finishing each iteration, check the sum and store the number of fails for each type of variable
 */
void DataRaceBugTest(int maxCount, int maxIterations);

/*
 * Test different approaches for running two threads producing/consuming some data simultaneously.
 */
void ProducerConsumerTest();

/*
 * Test reentrant functions acquiring the same lock
 */
void ReentrantLockTest();

/*
 * Assert that two threads sharing the same data doesn't overlap
 */
void UnnecessaryLockTest();

/*
 * Readers can access simultaneoulsy but writes are only allowed when no one is reading
 */
void ReaderWriterLockTest();
