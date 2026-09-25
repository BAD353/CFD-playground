#include "cuda/main.cuh"
#include "cuda/kernels.cuh"
#include <cstdint>
#include <cstdio>

void runJacobiPressureSolve(int n, int m, double *pressure, double *divergence, uint8_t *isSolid, int iterations, double dx) {
    int gridSize = n * m;
    double *cuda_pressure, *cuda_nextPressure, *cuda_divergence;
    uint8_t *cuda_isSolid;
    cudaMalloc(&cuda_pressure, gridSize * sizeof(double));
    cudaMalloc(&cuda_nextPressure, gridSize * sizeof(double));
    cudaMalloc(&cuda_divergence, gridSize * sizeof(double));
    cudaMalloc(&cuda_isSolid, gridSize * sizeof(uint8_t));

    cudaMemcpy(cuda_pressure, pressure, gridSize * sizeof(double), cudaMemcpyHostToDevice);
    cudaMemcpy(cuda_nextPressure, pressure, gridSize * sizeof(double), cudaMemcpyHostToDevice);
    cudaMemcpy(cuda_divergence, divergence, gridSize * sizeof(double), cudaMemcpyHostToDevice);
    cudaMemcpy(cuda_isSolid, isSolid, gridSize * sizeof(uint8_t), cudaMemcpyHostToDevice);

    dim3 block(32,8);
    dim3 grid((n + block.x - 1) / block.x, (m + block.y - 1) / block.y);

    for (int it = 0; it < iterations; it++) {
        applyPressureJacobiIteration<<<grid, block>>>(n, m, cuda_pressure, cuda_divergence, cuda_isSolid, cuda_nextPressure, dx);
        std::swap(cuda_pressure, cuda_nextPressure);
        applyPressureBoundaries<<<grid, block>>>(n, m, cuda_pressure, cuda_isSolid);
    }

    cudaDeviceSynchronize();

    cudaMemcpy(pressure, cuda_pressure, gridSize * sizeof(double), cudaMemcpyDeviceToHost);


    cudaFree(cuda_pressure);
    cudaFree(cuda_divergence);
    cudaFree(cuda_isSolid);
    cudaFree(cuda_nextPressure);

    // cudaError_t e1 = cudaGetLastError();
    // cudaError_t e2 = cudaDeviceSynchronize();
    // if (e1 != cudaSuccess || e2 != cudaSuccess)
    //      fprintf(stderr, "CUDA error: %s / %s\n", cudaGetErrorString(e1), cudaGetErrorString(e2));fprintf(stderr, "CUDA error: %s / %s\n", cudaGetErrorString(e1), cudaGetErrorString(e2));
}
