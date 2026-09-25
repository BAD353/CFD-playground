#include "cuda/main.cuh"
#include "cuda/kernels.cuh"

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

    int threadCount = gridSize;
    int blockSize = 256;
    int numBlocks = (threadCount + blockSize - 1) / blockSize;

    for (int it = 0; it < iterations; it++) {
        applyPressureJacobiIteration<<<numBlocks, blockSize>>>(n, m, cuda_pressure, cuda_divergence, cuda_isSolid, cuda_nextPressure, dx);
        std::swap(cuda_pressure, cuda_nextPressure);
        applyPressureBoundaries<<<numBlocks, blockSize>>>(n, m, cuda_pressure, cuda_isSolid);
    }

    cudaDeviceSynchronize();

    cudaMemcpy(pressure, cuda_pressure, gridSize * sizeof(double), cudaMemcpyDeviceToHost);


    cudaFree(cuda_pressure);
    cudaFree(cuda_divergence);
    cudaFree(cuda_isSolid);
    cudaFree(cuda_nextPressure);
}
