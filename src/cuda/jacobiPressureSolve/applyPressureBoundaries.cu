#include <cstdint>
#include "cuda/kernels.cuh"

__global__ void applyPressureBoundaries(int n, int m, float *pressure, uint8_t *isSolid) {
    int x = blockIdx.x * blockDim.x + threadIdx.x;
    int y = blockIdx.y * blockDim.y + threadIdx.y;

    if (x >= n || y >= m)
        return;

    if (x == 0)
        pressure[y * n + 0] = pressure[y * n + 1];
    if (x == n - 1)
        pressure[y * n + (n - 1)] = pressure[y * n + (n - 2)];

    if (y == 0)
        pressure[0 * n + x] = pressure[1 * n + x];
    if (y == m - 1)
        pressure[(m - 1) * n + x] = pressure[(m - 2) * n + x];

    if (isSolid[y * n + x]) pressure[y * n + x] = 0;
}
