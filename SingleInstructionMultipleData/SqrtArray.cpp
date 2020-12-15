#include "Tests.h"

#include <algorithm>
#include <chrono>

void SqrtArray_ref(float* __restrict r, const float* __restrict a, int count) {
    for (int i = 0; i < count; i++) {
        if(a[i] > 0.0f) {
            r[i] = std::sqrt(a[i]);
        } else {
            r[i] = 0.0f;
        }
    }
}

/*

void SqrtArray_sse_broken(float* __restrict r, const float* __restrict a, int count) {
    ASSERT(count % 4 == 0);
   
    // sse register with all zeros
    __m128 vz = _mm_set1_ps(0.0f);

    for (int i = 0; i < count; i += 4) {

        // va = {a[i], a[i+1], a[i+2], a[i+3]}
        __m128 va = _mm_load_ps(a + i);
        
        __m128 vr;
        // don't make sense because, if the result was a boolean
        // what element of the vector it would be referring for?
        if (_mm_cmpge_ps(va, vz)) {
            vr = _mm_sqrt_ps(va);
        } else {
            vr = vz;
        }

        _mm_store_ps(r + i, vr);
    }
}

*/

void SqrtArray_sse(float* __restrict r, const float* __restrict a, int count) {
    ASSERT(count % 4 == 0);

    // sse register with all zeros
    __m128 vz = _mm_set1_ps(0.0f);

    for (int i = 0; i < count; i += 4) {

        // va = {a[i], a[i+1], a[i+2], a[i+3]}
        __m128 va = _mm_load_ps(a + i);

        // always do the quotient, but it may end
        // up producing QNaN in some or all lanes
        __m128 vq = _mm_sqrt_ps(va);
            
        // now select between vr and vz, depending
        // on whether the input was greater than
        // or equal to zero or not
        __m128 mask = _mm_cmpge_ps(vq, vz);
        __m128 qmask = _mm_and_ps(mask, vq);
        __m128 znotmask = _mm_andnot_ps(mask, vz);
        
        // (vq & mask) | (vz & !mask)
        __m128 vr = _mm_or_ps(qmask, znotmask);

        // store the result in the result array
        _mm_store_ps(r + i, vr);
    }
}

void SqrtArrayTest() {

    constexpr int SIZE = 16000000;
    alignas(16) float* arrayA = new float[SIZE];
    alignas(16) float* arrayR = new float[SIZE];
    
    for (int i = 0; i < SIZE; i++) {
        arrayA[i] = 50 - (100 * rand() / RAND_MAX);
    }

    std::cout << "\n=========================================================================================================\n";
    std::cout << "\nTime spent to take the square root of an array with size " << SIZE << "\n\n";

    auto begin = std::chrono::high_resolution_clock::now();
    SqrtArray_ref(arrayR, arrayA, SIZE);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
    std::cout << "Serially: " << duration << " ms\n";
    
    begin = std::chrono::high_resolution_clock::now();
    SqrtArray_sse(arrayR, arrayA, SIZE);
    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
    std::cout << "SIMD 4 per pack: " << duration << " ms\n";

    std::cout << "\n=========================================================================================================\n";

    delete[] arrayA;
    delete[] arrayR;

}
