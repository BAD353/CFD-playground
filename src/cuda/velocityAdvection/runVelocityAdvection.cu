#include "cuda/kernels.cuh"
#include "cuda/main.cuh"
#include <cstdint>
#include <cstdio>

void runVelocityAdvection(int n, int m, double *horizontalVelocity, double *verticalVelocity, double *nextHorizontalVelocity,
                          double *nextVerticalVelocity, uint8_t *isSolid, double dx, double dy, double dt) {
    int gridSize = n * m;
    int horizontalGridSize = (n + 1) * m;
    int verticalGridSize = n * (m + 1);
    double *cuda_horizontalVelocity, *cuda_verticalVelocity, *cuda_nextHorizontalVelocity, *cuda_nextVerticalVelocity;
    uint8_t *cuda_isSolid;
    cudaMalloc(&cuda_horizontalVelocity, horizontalGridSize * sizeof(double));
    cudaMalloc(&cuda_verticalVelocity, verticalGridSize * sizeof(double));
    cudaMalloc(&cuda_nextHorizontalVelocity, horizontalGridSize * sizeof(double));
    cudaMalloc(&cuda_nextVerticalVelocity, verticalGridSize * sizeof(double));
    cudaMalloc(&cuda_isSolid, gridSize * sizeof(uint8_t));

    cudaMemcpy(cuda_horizontalVelocity, horizontalVelocity, horizontalGridSize * sizeof(double), cudaMemcpyHostToDevice);
    cudaMemcpy(cuda_verticalVelocity, verticalVelocity, verticalGridSize * sizeof(double), cudaMemcpyHostToDevice);
    cudaMemcpy(cuda_nextHorizontalVelocity, horizontalVelocity, horizontalGridSize * sizeof(double), cudaMemcpyHostToDevice);
    cudaMemcpy(cuda_nextVerticalVelocity, verticalVelocity, verticalGridSize * sizeof(double), cudaMemcpyHostToDevice);
    cudaMemcpy(cuda_isSolid, isSolid, gridSize * sizeof(uint8_t), cudaMemcpyHostToDevice);

    dim3 block(32, 8);
    dim3 grid((n + block.x - 1) / block.x, (m + block.y - 1) / block.y);

    applyHorizontalVelocityAdvection<<<grid, block>>>(n, m, cuda_horizontalVelocity, cuda_verticalVelocity, cuda_nextHorizontalVelocity, cuda_nextVerticalVelocity, cuda_isSolid, dx, dy, dt);
    applyVerticalVelocityAdvection<<<grid, block>>>(n, m, cuda_horizontalVelocity, cuda_verticalVelocity, cuda_nextHorizontalVelocity, cuda_nextVerticalVelocity, cuda_isSolid, dx, dy, dt);

    cudaDeviceSynchronize();

    cudaMemcpy(nextHorizontalVelocity, cuda_nextHorizontalVelocity, horizontalGridSize * sizeof(double), cudaMemcpyDeviceToHost);
    cudaMemcpy(nextVerticalVelocity, cuda_nextVerticalVelocity, verticalGridSize * sizeof(double), cudaMemcpyDeviceToHost);

    cudaFree(cuda_horizontalVelocity);
    cudaFree(cuda_verticalVelocity);
    cudaFree(cuda_isSolid);
    cudaFree(cuda_nextHorizontalVelocity);
    cudaFree(cuda_nextVerticalVelocity);

    // cudaError_t e1 = cudaGetLastError();
    // cudaError_t e2 = cudaDeviceSynchronize();
    // if (e1 != cudaSuccess || e2 != cudaSuccess)
    //      fprintf(stderr, "CUDA error: %s / %s\n", cudaGetErrorString(e1), cudaGetErrorString(e2));fprintf(stderr, "CUDA error:
    //      %s / %s\n", cudaGetErrorString(e1), cudaGetErrorString(e2));
}
