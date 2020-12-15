// SSE intrinsics
#include <xmmintrin.h>
#include <intrin.h>

#include "../HandlingErrors/Assert.h"

#define _MY_MM_TRANSPOSE4_PS(row0, row1, row2, row3) { \
\
    __m128 tmp0 = _mm_shuffle_ps((row0), (row1), 0x44); \
    __m128 tmp2 = _mm_shuffle_ps((row0), (row1), 0xEE); \
    __m128 tmp1 = _mm_shuffle_ps((row2), (row3), 0x44); \
    __m128 tmp3 = _mm_shuffle_ps((row2), (row3), 0xEE); \
\
    (row0) = _mm_shuffle_ps(tmp0, tmp1, 0x88); \
    (row1) = _mm_shuffle_ps(tmp0, tmp1, 0xDD); \
    (row2) = _mm_shuffle_ps(tmp2, tmp3, 0x88); \
    (row3) = _mm_shuffle_ps(tmp2, tmp3, 0xDD); \
} \

/*
 * Create the bit mask for shuffling sse vectors
 * r = shuffle(a, b, SHUFMASK(p, q, r, s))
 * r == {a[p], a[q], b[r], b[s]}
 */
constexpr unsigned int _MY_SHUFMASK(unsigned int p, unsigned int q, unsigned int r, unsigned int s) {
    return (p | (q << 2) | (r << 4) | (s << 6));
}

// measure the time it takes to sum two arrays serially vs vectorized by SIMD
void AddArraysTest();

// measure the time it takes to make the dot product of two arrays with different methods
void DotArraysTest();

// test VecMatrix and MatrixMatrix multiplications
void MulMatrixTest();

// measure the time it takes to get the square root of each element of an array
void SqrtArrayTest();
