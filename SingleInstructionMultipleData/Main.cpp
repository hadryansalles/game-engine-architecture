#include "Tests.h"

int main() {

    SqrtArrayTest();
    MulMatrixTest();
    DotArraysTest();
    AddArraysTest();

    // a = [4, 3, 2, 1]
    __m128 a = _mm_set_ps(1, 2, 3, 4);

    // b = [5, 6, 7, 8]
    float pDataB[] = { 5, 6, 7, 8 };
    __m128 b = _mm_load_ps(pDataB);

    // pDataA = a
    float pDataA[4];
    _mm_store_ps(pDataA, a);

    // c = [9, 9, 9, 9]
    __m128 c = _mm_add_ps(a, b);

    // d = [36, 27, 18, 9]
    __m128 d = _mm_mul_ps(a, c);

    return 0;
}
