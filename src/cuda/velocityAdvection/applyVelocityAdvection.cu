#include "cuda/kernels.cuh"
#include "cuda/main.cuh"
#include <cstdint>
#include <cstdio>
const int STEP_COUNT = 10;

__device__ double clampDouble(double x, double a, double b) {
    if (x < a) return a;
    if (b < x) return b;
    return x;
}

__device__ bool isPositionSolid(int n, int m, double gx, double gy, double dx, double dy, uint8_t *isSolid) {
    int cx = static_cast<int>(std::floor(gx / dx));
    int cy = static_cast<int>(std::floor(gy / dy));
    cx = clampDouble(cx, 0, n - 1);
    cy = clampDouble(cy, 0, m - 1);
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

__device__ double sampleHorizontalVelocity(int n, int m, double worldX, double worldY, double dx, double dy, double *field,
                                           uint8_t *isSolid) {
    double gx = worldX / dx;
    double gy = worldY / dy - 0.5;

    gx = clampDouble(gx, 0.0, (n + 1) - 1.001);
    gy = clampDouble(gy, 0.0, m - 1.001);

    int x0 = static_cast<int>(std::floor(gx));
    int y0 = static_cast<int>(std::floor(gy));
    int x1 = x0 + 1;
    int y1 = y0 + 1;

    double fx = gx - x0;
    double fy = gy - y0;

    double w00 = (1 - fx) * (1 - fy);
    double w10 = fx * (1 - fy);
    double w01 = (1 - fx) * fy;
    double w11 = fx * fy;

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

__device__ double sampleVerticalVelocity(int n, int m, double worldX, double worldY, double dx, double dy, double *field,
                                           uint8_t *isSolid) {
    double gx = worldX / dx - 0.5;
    double gy = worldY / dy;

    gx = clampDouble(gx, 0.0, n - 1.001);
    gy = clampDouble(gy, 0.0, (m + 1) - 1.001);

    int x0 = static_cast<int>(std::floor(gx));
    int y0 = static_cast<int>(std::floor(gy));
    int x1 = x0 + 1;
    int y1 = y0 + 1;

    double fx = gx - x0;
    double fy = gy - y0;

    double w00 = (1 - fx) * (1 - fy);
    double w10 = fx * (1 - fy);
    double w01 = (1 - fx) * fy;
    double w11 = fx * fy;

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

__global__ void applyHorizontalVelocityAdvection(int n, int m, double *horizontalVelocity, double *verticalVelocity,
                                                 double *nextHorizontalVelocity, double *nextVerticalVelocity, uint8_t *isSolid,
                                                 double dx, double dy, double dt) {
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

    double posX = x * dx;
    double posY = (y+0.5) * dy;

    for (int step = 0; step < STEP_COUNT; step++) {

        double u = sampleHorizontalVelocity(n, m, posX, posY, dx, dy, horizontalVelocity, isSolid);

        double v = sampleVerticalVelocity(n, m, posX, posY, dx, dy, verticalVelocity, isSolid);

        double new_posX = posX - u * (dt / STEP_COUNT);
        double new_posY = posY - v * (dt / STEP_COUNT);
        if (isPositionSolid(n, m, new_posX, new_posY, dx, dy, isSolid))
            break;
        posX = new_posX;
        posY = new_posY;
    }
    nextHorizontalVelocity[y * (n + 1) + x] = sampleHorizontalVelocity(n, m, posX, posY, dx, dy, horizontalVelocity, isSolid);
}

__global__ void applyVerticalVelocityAdvection(int n, int m, double *horizontalVelocity, double *verticalVelocity,
                                               double *nextHorizontalVelocity, double *nextVerticalVelocity, uint8_t *isSolid,
                                               double dx, double dy, double dt) {
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

    double posX = (x + 0.5) * dx;
    double posY = y * dy;

    for (int step = 0; step < STEP_COUNT; step++) {

        double u = sampleHorizontalVelocity(n, m, posX, posY, dx, dy, horizontalVelocity, isSolid);

        double v = sampleVerticalVelocity(n, m, posX, posY, dx, dy, verticalVelocity, isSolid);

        double new_posX = posX - u * (dt / STEP_COUNT);
        double new_posY = posY - v * (dt / STEP_COUNT);
        if (isPositionSolid(n, m, new_posX, new_posY, dx, dy, isSolid))
            break;
        posX = new_posX;
        posY = new_posY;
    }
    nextVerticalVelocity[y * n + x] = sampleVerticalVelocity(n, m, posX, posY, dx, dy, verticalVelocity, isSolid);
}
