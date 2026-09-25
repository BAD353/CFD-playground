#pragma once

__global__ void applyPressureJacobiIteration(int n, int m, double *pressure, double *divergence, bool *isSolid, double *nextPressure, double dx);
__global__ void applyPressureBoundaries(int n, int m, double *pressure, bool *isSolid);