#include "cuda/kernels.cuh"
#include <cstdint>

__global__ void initPressureJacobiIteration(int n, int m, double *inverseNeighbourCount, double *divergenceTerm, double *pressure,
                                            double *nextPressure, double *divergence, uint8_t *isSolid, double dx) {
    int x = blockIdx.x * blockDim.x + threadIdx.x;
    int y = blockIdx.y * blockDim.y + threadIdx.y;

    if (x < 1)
        return;
    if (y < 1)
        return;
    if (x >= n - 1)
        return;
    if (y >= m - 1)
        return;

    int ind = y * n + x;

    inverseNeighbourCount[ind] = 0;
    divergenceTerm[ind] = 0;

    if (isSolid[ind] == true) {
        pressure[ind] = 0;
        nextPressure[ind] = 0;
        return;
    }
    int count = 0;
    if (isSolid[ind + 1] == false)
        count++;
    if (isSolid[ind - 1] == false)
        count++;
    if (isSolid[ind + n] == false)
        count++;
    if (isSolid[ind - n] == false)
        count++;
    if (count > 0) {
        inverseNeighbourCount[ind] = 1.0 / count;
        divergenceTerm[ind] = -dx * dx * divergence[ind] * inverseNeighbourCount[ind];
    }
}

__global__ void applyPressureJacobiIteration(int n, int m, double *inverseNeighbourCount, double *divergenceTerm,
                                             double *pressure, double *nextPressure, double dx) {
    int x = blockIdx.x * blockDim.x + threadIdx.x;
    int y = blockIdx.y * blockDim.y + threadIdx.y;

    if (x < 1)
        return;
    if (y < 1)
        return;
    if (x >= n - 1)
        return;
    if (y >= m - 1)
        return;
    int i =  y * n + x;
    nextPressure[i] = (pressure[i+1] + pressure[i-1] + pressure[i+n] + pressure[i-n]) * inverseNeighbourCount[i] + divergenceTerm[i];
}