#include "cuda/kernels.cuh"
#include "cuda/main.cuh"
#include <cstdint>
#include <cstdio>
const int STEP_COUNT = 10;

__device__ float clampfloat(float x, float a, float b) {
    if (x < a) return a;
    if (b < x) return b;
    return x;
}

__device__ bool isPositionSolid(int n, int m, float gx, float gy, float dx, float dy, uint8_t *isSolid) {
    int cx = static_cast<int>(std::floor(gx / dx));
    int cy = static_cast<int>(std::floor(gy / dy));
    cx = clampfloat(cx, 0, n - 1);
    cy = clampfloat(cy, 0, m - 1);
    return isSolid[cy * n + cx];
}

__device__ bool isHorizontalFaceSolid(int n, int m, int x, int y, uint8_t *isSolid) {
    if (x > 0 && isSolid[y * n + (x - 1)])
        return true;
    if (x < n && isSolid[y * n + x])
        return true;
    return false;
}

__device__ bool isVerticalFaceSolid(int n, int m, int x, int y, uint8_t *isSolid) {
    if (y > 0 && isSolid[(y - 1) * n + x])
        return true;
    if (y < m && isSolid[y * n + x])
        return true;
    return false;
}

__device__ float sampleHorizontalVelocity(int n, int m, float worldX, float worldY, float dx, float dy, float *field,
                                           uint8_t *isSolid) {
    float gx = worldX / dx;
    float gy = worldY / dy - 0.5f;

    gx = clampfloat(gx, 0.0f, (n + 1) - 1.001f);
    gy = clampfloat(gy, 0.0f, m - 1.001f);

    int x0 = static_cast<int>(std::floor(gx));
    int y0 = static_cast<int>(std::floor(gy));
    int x1 = x0 + 1;
    int y1 = y0 + 1;

    float fx = gx - x0;
    float fy = gy - y0;

    float w00 = (1 - fx) * (1 - fy);
    float w10 = fx * (1 - fy);
    float w01 = (1 - fx) * fy;
    float w11 = fx * fy;

    if (isHorizontalFaceSolid(n, m, x0, y0, isSolid))
        w00 = 0;
    if (isHorizontalFaceSolid(n, m, x1, y0, isSolid))
        w10 = 0;
    if (isHorizontalFaceSolid(n, m, x0, y1, isSolid))
        w01 = 0;
    if (isHorizontalFaceSolid(n, m, x1, y1, isSolid))
        w11 = 0;

    // Note: divide by total to renormalize
    return (w00 * field[y0 * (n + 1) + x0] + w10 * field[y0 * (n + 1) + x1] + w01 * field[y1 * (n + 1) + x0] +
            w11 * field[y1 * (n + 1) + x1]) /
           (w00 + w10 + w01 + w11);
}

__device__ float sampleVerticalVelocity(int n, int m, float worldX, float worldY, float dx, float dy, float *field,
                                           uint8_t *isSolid) {
    float gx = worldX / dx - 0.5f;
    float gy = worldY / dy;

    gx = clampfloat(gx, 0.0f, n - 1.001f);
    gy = clampfloat(gy, 0.0f, (m + 1) - 1.001f);

    int x0 = static_cast<int>(std::floor(gx));
    int y0 = static_cast<int>(std::floor(gy));
    int x1 = x0 + 1;
    int y1 = y0 + 1;

    float fx = gx - x0;
    float fy = gy - y0;

    float w00 = (1 - fx) * (1 - fy);
    float w10 = fx * (1 - fy);
    float w01 = (1 - fx) * fy;
    float w11 = fx * fy;

    if (isVerticalFaceSolid(n, m, x0, y0, isSolid))
        w00 = 0;
    if (isVerticalFaceSolid(n, m, x1, y0, isSolid))
        w10 = 0;
    if (isVerticalFaceSolid(n, m, x0, y1, isSolid))
        w01 = 0;
    if (isVerticalFaceSolid(n, m, x1, y1, isSolid))
        w11 = 0;

    // Note: divide by total to renormalize
    return (w00 * field[y0 * n + x0] + w10 * field[y0 * n + x1] + w01 * field[y1 * n + x0] +
            w11 * field[y1 * n + x1]) /
           (w00 + w10 + w01 + w11);
}

__global__ void applyHorizontalVelocityAdvection(int n, int m, float *horizontalVelocity, float *verticalVelocity,
                                                 float *nextHorizontalVelocity, float *nextVerticalVelocity, uint8_t *isSolid,
                                                 float dx, float dy, float dt) {
    int x = blockIdx.x * blockDim.x + threadIdx.x;
    int y = blockIdx.y * blockDim.y + threadIdx.y;

    if (x < 1)
        return;
    if (y < 0)
        return;
    if (x >= n)
        return;
    if (y >= m)
        return;

    if (isHorizontalFaceSolid(n, m, x, y, isSolid))
        return;

    float posX = x * dx;
    float posY = (y+0.5f) * dy;

    for (int step = 0; step < STEP_COUNT; step++) {

        float u = sampleHorizontalVelocity(n, m, posX, posY, dx, dy, horizontalVelocity, isSolid);

        float v = sampleVerticalVelocity(n, m, posX, posY, dx, dy, verticalVelocity, isSolid);

        float new_posX = posX - u * (dt / STEP_COUNT);
        float new_posY = posY - v * (dt / STEP_COUNT);
        if (isPositionSolid(n, m, new_posX, new_posY, dx, dy, isSolid))
            break;
        posX = new_posX;
        posY = new_posY;
    }
    nextHorizontalVelocity[y * (n + 1) + x] = sampleHorizontalVelocity(n, m, posX, posY, dx, dy, horizontalVelocity, isSolid);
}

__global__ void applyVerticalVelocityAdvection(int n, int m, float *horizontalVelocity, float *verticalVelocity,
                                               float *nextHorizontalVelocity, float *nextVerticalVelocity, uint8_t *isSolid,
                                               float dx, float dy, float dt) {
    int x = blockIdx.x * blockDim.x + threadIdx.x;
    int y = blockIdx.y * blockDim.y + threadIdx.y;

    if (x < 0)
        return;
    if (y < 1)
        return;
    if (x >= n)
        return;
    if (y >= m)
        return;

    if (isVerticalFaceSolid(n, m, x, y, isSolid))
        return;

    float posX = (x + 0.5f) * dx;
    float posY = y * dy;

    for (int step = 0; step < STEP_COUNT; step++) {

        float u = sampleHorizontalVelocity(n, m, posX, posY, dx, dy, horizontalVelocity, isSolid);

        float v = sampleVerticalVelocity(n, m, posX, posY, dx, dy, verticalVelocity, isSolid);

        float new_posX = posX - u * (dt / STEP_COUNT);
        float new_posY = posY - v * (dt / STEP_COUNT);
        if (isPositionSolid(n, m, new_posX, new_posY, dx, dy, isSolid))
            break;
        posX = new_posX;
        posY = new_posY;
    }
    nextVerticalVelocity[y * n + x] = sampleVerticalVelocity(n, m, posX, posY, dx, dy, verticalVelocity, isSolid);
}
