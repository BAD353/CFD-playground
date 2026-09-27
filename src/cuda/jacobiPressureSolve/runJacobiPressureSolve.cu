#include "cuda/main.cuh"
#include "cuda/kernels.cuh"
#include <cstdint>
#include <cstdio>

void runJacobiPressureSolve(int n, int m, float *pressure, float *divergence, uint8_t *isSolid, int iterations, float dx) {
    int gridSize = n * m;
    float *cuda_pressure, *cuda_nextPressure, *cuda_divergence;
    uint8_t *cuda_isSolid;
    cudaMalloc(&cuda_pressure, gridSize * sizeof(float));
    cudaMalloc(&cuda_nextPressure, gridSize * sizeof(float));
    cudaMalloc(&cuda_divergence, gridSize * sizeof(float));
    cudaMalloc(&cuda_isSolid, gridSize * sizeof(uint8_t));

    float *cuda_inverseNeighbourCount, *cuda_divergenceTerm;
    cudaMalloc(&cuda_inverseNeighbourCount, gridSize * sizeof(float));
    cudaMalloc(&cuda_divergenceTerm, gridSize * sizeof(float));

    cudaMemcpy(cuda_pressure, pressure, gridSize * sizeof(float), cudaMemcpyHostToDevice);
    cudaMemcpy(cuda_nextPressure, pressure, gridSize * sizeof(float), cudaMemcpyHostToDevice);
    cudaMemcpy(cuda_divergence, divergence, gridSize * sizeof(float), cudaMemcpyHostToDevice);
    cudaMemcpy(cuda_isSolid, isSolid, gridSize * sizeof(uint8_t), cudaMemcpyHostToDevice);

    dim3 block(32,8);
    dim3 grid((n + block.x - 1) / block.x, (m + block.y - 1) / block.y);

    initPressureJacobiIteration<<<grid, block>>>(n, m, cuda_inverseNeighbourCount, cuda_divergenceTerm, cuda_pressure, cuda_nextPressure,  cuda_divergence, cuda_isSolid, dx);

    for (int it = 0; it < iterations; it++) {
        applyPressureJacobiIteration<<<grid, block>>>(n, m, cuda_inverseNeighbourCount, cuda_divergenceTerm, cuda_pressure, cuda_nextPressure, dx);
        std::swap(cuda_pressure, cuda_nextPressure);
        applyPressureBoundaries<<<grid, block>>>(n, m, cuda_pressure, cuda_isSolid);
    }

    cudaDeviceSynchronize();

    cudaMemcpy(pressure, cuda_pressure, gridSize * sizeof(float), cudaMemcpyDeviceToHost);


    cudaFree(cuda_pressure);
    cudaFree(cuda_divergence);
    cudaFree(cuda_isSolid);
    cudaFree(cuda_nextPressure);
    cudaFree(cuda_inverseNeighbourCount);
    cudaFree(cuda_divergenceTerm);

    // cudaError_t e1 = cudaGetLastError();
    // cudaError_t e2 = cudaDeviceSynchronize();
    // if (e1 != cudaSuccess || e2 != cudaSuccess)
    //      fprintf(stderr, "CUDA error: %s / %s\n", cudaGetErrorString(e1), cudaGetErrorString(e2));fprintf(stderr, "CUDA error: %s / %s\n", cudaGetErrorString(e1), cudaGetErrorString(e2));
}
