#pragma once
#include <cstdint>

__global__ void initPressureJacobiIteration(int n, int m, float *inverseNeighbourCount, float *divergenceTerm, float *pressure,
                                            float *nextPressure, float *divergence, uint8_t *isSolid, float dx);
__global__ void applyPressureJacobiIteration(int n, int m, float *inverseNeighbourCount, float *divergenceTerm,
                                             float *pressure, float *nextPressure, float dx);
__global__ void applyPressureBoundaries(int n, int m, float *pressure, uint8_t *isSolid);

__global__ void applyHorizontalVelocityAdvection(int n, int m, float *horizontalVelocity, float *verticalVelocity,
                                                 float *nextHorizontalVelocity, float *nextVerticalVelocity, uint8_t *isSolid,
                                                 float dx, float dy, float dt);
__global__ void applyVerticalVelocityAdvection(int n, int m, float *horizontalVelocity, float *verticalVelocity,
                                               float *nextHorizontalVelocity, float *nextVerticalVelocity, uint8_t *isSolid,
                                               float dx, float dy, float dt);
