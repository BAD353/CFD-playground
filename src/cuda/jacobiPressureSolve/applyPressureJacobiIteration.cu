#include "cuda/kernels.cuh"
#include <cstdint>

__global__ void applyPressureJacobiIteration(int n, int m, double *pressure, double *divergence, uint8_t *isSolid,
                                             double *nextPressure, double dx) {
    int x = blockIdx.x * blockDim.x + threadIdx.x;
    int y = blockIdx.y * blockDim.y + threadIdx.y;

    if (x < 1) return;
    if (y < 1) return;
    if (x >= n-1) return;
    if (y >= m-1) return;
    if (isSolid[y * n + x] == true) {
        nextPressure[y * n + x] = 0;
        return;
    }
    int count = 0;
    double sum = 0;
    if (isSolid[y * n + (x + 1)] == false) {
        sum += pressure[y * n + (x + 1)];
        count++;
    }
    if (isSolid[y * n + (x - 1)] == false) {
        sum += pressure[y * n + (x - 1)];
        count++;
    }
    if (isSolid[(y + 1) * n + x] == false) {
        sum += pressure[(y + 1) * n + x];
        count++;
    }
    if (isSolid[(y - 1) * n + x] == false) {
        sum += pressure[(y - 1) * n + x];
        count++;
    }
    if (count == 0)
        nextPressure[y * n + x] = 0;
    else
        nextPressure[y * n + x] = (sum - dx * dx * divergence[y * n + x]) / count;
}