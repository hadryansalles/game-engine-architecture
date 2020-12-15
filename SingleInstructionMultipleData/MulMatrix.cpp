#include "Tests.h"

union Mat44 {
    float c[4][4];
    __m128 row[4];
};

__m128 MulVecMat_sse(const __m128& v, const Mat44& M) {
    // first transpose v
    __m128 vX = _mm_shuffle_ps(v, v, 0x00); // (vx, vx, vx, vx)
    __m128 vY = _mm_shuffle_ps(v, v, 0x55); // (vy, vy, vy, vy)
    __m128 vZ = _mm_shuffle_ps(v, v, 0xAA); // (vz, vz, vz, vz)
    __m128 vW = _mm_shuffle_ps(v, v, 0xFF); // (vw, vw, vw, vw)

    // multiply by each row and add
    __m128 r =        _mm_mul_ps(vX, M.row[0]);
    r = _mm_add_ps(r, _mm_mul_ps(vY, M.row[1]));
    r = _mm_add_ps(r, _mm_mul_ps(vZ, M.row[2]));
    r = _mm_add_ps(r, _mm_mul_ps(vW, M.row[3]));

    return r;
}

void MulMatMat_sse(Mat44& R, const Mat44& A, const Mat44& B) {
    R.row[0] = MulVecMat_sse(A.row[0], B);
    R.row[1] = MulVecMat_sse(A.row[1], B);
    R.row[2] = MulVecMat_sse(A.row[2], B);
    R.row[3] = MulVecMat_sse(A.row[3], B);
}

std::ostream& operator<<(std::ostream& os, Mat44& mat) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            os << mat.c[i][j] << ", ";
        }
        os << "\n";
    }
    return os;
}

void MulMatrixTest() {

    std::cout << "\n=========================================================================================================\n";
    std::cout << "\nMultiplying Vector and Matrix\n";

    Mat44 matA;
    Mat44 matB;
    float vecA[4];

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            matA.c[i][j] = 10 * rand() / RAND_MAX;
            matB.c[i][j] = 10 * rand() / RAND_MAX;
        }
        vecA[i] = 10 * rand() / RAND_MAX;
    }
    
    __m128 vecA_sse = _mm_load_ps(vecA);
    __m128 vecMatRes = MulVecMat_sse(vecA_sse, matA);
   
    Mat44 matRes;
    MulMatMat_sse(matRes, matA, matB);
    
    std::cout << "\nMat A:\n" << matA;
    std::cout << "\nMat B:\n" << matB;
    std::cout << "\nRes:\n" << matRes;


    std::cout << "\n=========================================================================================================\n";
}