#include "cuda_runtime.h"
#include "device_launch_parameters.h"

#include <stdio.h>

#include <chrono>
#include <iostream>

#include "../HandlingErrors/Assert.h"
#include "../VisualStudio/ScopedInstrument.h"

void DotArrays_ref(int count, float* r, const float* a, const float* b) {
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

void CmpArrays_ref(int count, const float* a, const float* b) {
    for (int i = 0; i < count; ++i) {
        if (a[i] != b[i]) {
            DEBUG_BREAK();
        }
    }
}

__global__ void CmpKernel_CUDA(int count, const float* a, const float* b) {

    int i = blockDim.x*blockIdx.x + threadIdx.x;
    
    if (i < count) {
        if (a[i] != b[i]) {
            DEBUG_BREAK();
        }
    }
}

__global__ void DotKernel_CUDA(float* r, const float* a, const float* b, unsigned int count) {

    // CUDA provides a thread index to each invocation
    // of the kernel, this is our loop i
    int i = blockDim.x*blockIdx.x + threadIdx.x;
    
    if (i < count) {
        // treat each block of four floats as a 
        // single four-element vector
        const unsigned j = i * 4;
        r[i] = a[j + 0] * b[j + 0]
             + a[j + 1] * b[j + 1]
             + a[j + 2] * b[j + 2]
             + a[j + 3] * b[j + 3];
    }
}

int main() {

    unsigned int SIZE = 16000000;
    unsigned int SIZE_R = SIZE/4;
    float* arrayA = new float[SIZE];
    float* arrayB = new float[SIZE];
    float* arrayC = new float[SIZE_R];
    float* arrayC_CUDA = new float[SIZE_R];

    cudaError_t cudaStatus;

    for (int i = 0; i < SIZE; i++) {
        arrayA[i] = 10 * rand() / RAND_MAX;
        arrayB[i] = 10 * rand() / RAND_MAX;
    }

    // allocate managed buffers that are visible to both GPU and CPU
    float* cr, *cr_ref, * ca, * cb;
    {
        ScopedInstrument instr("\nTime spent to allocate CUDA managed resources and copy input buffers: ");

        cudaSetDevice(0);

        cudaMalloc((void**)&ca, SIZE * sizeof(float));
        cudaMalloc((void**)&cb, SIZE * sizeof(float));
        cudaMalloc((void**)&cr, SIZE_R * sizeof(float));
        cudaMalloc((void**)&cr_ref, SIZE_R * sizeof(float));

        // transfer the data into GPU-visible memory
        cudaMemcpy(ca, arrayA, SIZE * sizeof(float), cudaMemcpyHostToDevice);
        cudaMemcpy(cb, arrayB, SIZE * sizeof(float), cudaMemcpyHostToDevice);

    }
    std::cout << "\n=========================================================================================================\n";

    std::cout << "\n=========================================================================================================\n";
    std::cout << "\nTime spent to make dot product of 2 arrays of size: " << SIZE << "\n\n";

    // ref
    {
        ScopedInstrument instr("Serial execution: ");
        DotArrays_ref(SIZE_R, arrayC, arrayA, arrayB);
    }

    int threadsPerBlock = 128;
    int blocksPerGrid = (SIZE_R + threadsPerBlock - 1) / threadsPerBlock;

    // cuda
    {
        ScopedInstrument instr("CUDA execution: ");

        // run the kernel on the GP
        DotKernel_CUDA<<<blocksPerGrid, threadsPerBlock>>>(cr, ca, cb, SIZE_R);

        cudaStatus = cudaGetLastError();
        if (cudaStatus != cudaSuccess) {
            std::cout << cudaGetErrorString(cudaStatus) << "\n";
            DEBUG_BREAK();
        }

        // wait for the GPU to finish
        cudaStatus = cudaDeviceSynchronize();
        
        if (cudaStatus != cudaSuccess) {
            std::cout << cudaGetErrorString(cudaStatus) << "\n";
            DEBUG_BREAK();
        }
    }
        
    cudaMemcpy(arrayC_CUDA, cr, SIZE_R * sizeof(float), cudaMemcpyDeviceToHost);
    cudaMemcpy(cr_ref, arrayC, SIZE_R * sizeof(float), cudaMemcpyHostToDevice);

    // compare the two result buffers serially
    {
        ScopedInstrument instr("Serial comparison: ");
        CmpArrays_ref(SIZE_R, arrayC, arrayC_CUDA);
    }
    std::cout << "\n=========================================================================================================\n";

    // compare the two result buffers with cuda
    {
        ScopedInstrument instr("CUDA comparison: ");
        // run the kernel on the GP
        CmpKernel_CUDA<<<blocksPerGrid, threadsPerBlock>>>(SIZE_R, cr, cr_ref);

        // wait for the GPU to finish
        cudaDeviceSynchronize();

    }
    std::cout << "\n=========================================================================================================\n";

    {
        std::cout << "\n=========================================================================================================\n";
        ScopedInstrument instr("\nTime spent to deallocate CUDA managed resources: ");

        cudaFree(cr_ref);
        cudaFree(cr);
        cudaFree(ca);
        cudaFree(cb);

    }
    std::cout << "\n=========================================================================================================\n";

    delete[] arrayA;
    delete[] arrayB;
    delete[] arrayC;
    delete[] arrayC_CUDA;

    return 0;
}
