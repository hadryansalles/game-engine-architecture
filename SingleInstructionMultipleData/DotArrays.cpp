#include "Tests.h"

#include <algorithm>
#include <chrono>
#include <iostream>

void DotArrays_ref(int count, float r[], const float a[], const float b[]) {
    for (int i = 0; i < count; ++i) {
        const int j = i * 4;
        // treat each block of four floats as a 
        // single four-element vector
        r[i] = a[j + 0] * b[j + 0]
             + a[j + 1] * b[j + 1]
             + a[j + 2] * b[j + 2]
             + a[j + 3] * b[j + 3];
    }
}

void DotArrays_sse_horizontal(int count, float r[], const float a[], const float b[]) {
    for (int i = 0; i < count; i++) {
        // treat each block of four floats as a
        // single four-element vector
        const int j = i * 4;

        __m128 va = _mm_load_ps(&a[j]);
        __m128 vb = _mm_load_ps(&b[j]);

        __m128 v0 = _mm_mul_ps(va, vb);

        // add across the register
        __m128 v1 = _mm_hadd_ps(v0, v0);
        __m128 vr = _mm_hadd_ps(v1, v1);

        _mm_store_ps(&r[i], vr);
    }
}

void DotArrays_sse_transpose(int count, float r[], const float a[], const float b[]) {
    for (int i = 0; i < count; i += 4) {

        __m128 vaX = _mm_load_ps(&a[(i + 0) * 4]); // a[0, 1, 2, 3]
        __m128 vaY = _mm_load_ps(&a[(i + 1) * 4]); // a[4, 5, 6, 7]
        __m128 vaZ = _mm_load_ps(&a[(i + 2) * 4]); // a[8, 9, 10, 11]
        __m128 vaW = _mm_load_ps(&a[(i + 3) * 4]); // a[12, 13, 14, 15]

        __m128 vbX = _mm_load_ps(&b[(i + 0) * 4]); // b[0, 1, 2, 3]
        __m128 vbY = _mm_load_ps(&b[(i + 1) * 4]); // b[4, 5, 6, 7]
        __m128 vbZ = _mm_load_ps(&b[(i + 2) * 4]); // b[8, 9, 10, 11]
        __m128 vbW = _mm_load_ps(&b[(i + 3) * 4]); // b[12, 13, 14, 15]

        _MM_TRANSPOSE4_PS(vaX, vaY, vaZ, vaW);
        _MM_TRANSPOSE4_PS(vbX, vbY, vbZ, vbW);
    
        __m128 result;
        result = _mm_mul_ps(vaX, vbX);
        result = _mm_add_ps(result, _mm_mul_ps(vaY, vbY));
        result = _mm_add_ps(result, _mm_mul_ps(vaZ, vbZ));
        result = _mm_add_ps(result, _mm_mul_ps(vaW, vbW));

        _mm_store_ps(&r[i], result);
    }
}

void DotKernel(unsigned i, float r[], const float a[], const float b[]) {
    const unsigned j = i * 4;
    r[i] = a[j + 0] * b[j + 0]
         + a[j + 1] * b[j + 1]
         + a[j + 2] * b[j + 2]
         + a[j + 3] * b[j + 3];
}

void DotArrays_gpgpu(unsigned count, float r[], const float a[], const float b[]) {
    for (int i = 0; i < count; ++i) {
        DotKernel(i, r, a, b);
    }
}

void DotArraysTest() {

    constexpr int SIZE = 16000000;
    alignas(16) float* arrayA = new float[SIZE];
    alignas(16) float* arrayB = new float[SIZE];
    alignas(16) float* arrayC = new float[SIZE];

    for (int i = 0; i < SIZE; i++) {
        arrayA[i] = 10 * rand() / RAND_MAX;
        arrayB[i] = 10 * rand() / RAND_MAX;
    }
    
    std::cout << "\n=========================================================================================================\n";
    std::cout << "\nTime spent to make dot product in 2 arrays of size " << SIZE << "\n\n";

    auto begin = std::chrono::high_resolution_clock::now();
    DotArrays_ref(SIZE/4, arrayC, arrayA, arrayB);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
    std::cout << "Serially: " << duration << " ms\n";
    
    begin = std::chrono::high_resolution_clock::now();
    DotArrays_sse_horizontal(SIZE/4, arrayC, arrayA, arrayB);
    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
    std::cout << "SSE Horizontal: " << duration << " ms\n";
    
    begin = std::chrono::high_resolution_clock::now();
    DotArrays_sse_transpose(SIZE/4, arrayC, arrayA, arrayB);
    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
    std::cout << "SSE Transposing: " << duration << " ms\n";
    
    std::cout << "\n=========================================================================================================\n";

    delete[] arrayA;
    delete[] arrayB;
    delete[] arrayC;
}
