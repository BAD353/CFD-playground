#pragma once
#include <cstdint>

__global__ void applyPressureJacobiIteration(int n, int m, double *pressure, double *divergence, uint8_t *isSolid, double *nextPressure, double dx);
__global__ void applyPressureBoundaries(int n, int m, double *pressure, uint8_t *isSolid);
