#pragma once
#include <cstdint>

__global__ void initPressureJacobiIteration(int n, int m, double *inverseNeighbourCount, double *divergenceTerm, double *pressure,
                                            double *nextPressure, double *divergence, uint8_t *isSolid, double dx);
__global__ void applyPressureJacobiIteration(int n, int m, double *inverseNeighbourCount, double *divergenceTerm,
                                             double *pressure, double *nextPressure, double dx);
__global__ void applyPressureBoundaries(int n, int m, double *pressure, uint8_t *isSolid);

__global__ void applyHorizontalVelocityAdvection(int n, int m, double *horizontalVelocity, double *verticalVelocity,
                                                 double *nextHorizontalVelocity, double *nextVerticalVelocity, uint8_t *isSolid,
                                                 double dx, double dy, double dt);
__global__ void applyVerticalVelocityAdvection(int n, int m, double *horizontalVelocity, double *verticalVelocity,
                                               double *nextHorizontalVelocity, double *nextVerticalVelocity, uint8_t *isSolid,
                                               double dx, double dy, double dt);
