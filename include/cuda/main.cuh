#pragma once
#include <cstdint>

void runJacobiPressureSolve(int n, int m, double *pressure, double *divergence, uint8_t *isSolid, int iterations, double dx); 
