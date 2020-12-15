#include "Tests.h"

#include <algorithm>
#include <chrono>
#include <iostream>

#include <stdlib.h>

void ArrayAdd(float* a, float* b, float* c, int n) {
    for (int i = 0; i < n; i++) {
        c[i] = a[i] + b[i];
    }
}

void ArrayAddSIMD(float* a, float* b, float* c, int n) {

    ASSERT(n % 4 == 0);

    for (int i = 0; i < n; i += 4) {
        __m128 aPack = _mm_load_ps(&a[i]);
        __m128 bPack = _mm_load_ps(&b[i]);
        __m128 result = _mm_add_ps(aPack, bPack);
        _mm_store_ps(&c[i], result);
    }
}

void AddArraysTest() {
    constexpr int SIZE = 16000000;
    alignas(16) float* arrayA = new float[SIZE];
    alignas(16) float* arrayB = new float[SIZE];
    alignas(16) float* arrayC = new float[SIZE];

    for (int i = 0; i < SIZE; i++) {
        arrayA[i] = 10 * rand() / RAND_MAX;
        arrayB[i] = 10 * rand() / RAND_MAX;
    }

    std::cout << "\n=========================================================================================================\n";
    std::cout << "\nTime spent to add two arrays of size " << SIZE << "\n\n";

    auto begin = std::chrono::high_resolution_clock::now();
    ArrayAdd(arrayA, arrayB, arrayC, SIZE);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
    std::cout << "Serially: " << duration << " ms\n";

    begin = std::chrono::high_resolution_clock::now();
    ArrayAddSIMD(arrayA, arrayB, arrayC, SIZE);
    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
    std::cout << "SIMD 4 per pack: " << duration << " ms\n";

    std::cout << "\n=========================================================================================================\n";

    delete[] arrayA;
    delete[] arrayB;
    delete[] arrayC;
}
