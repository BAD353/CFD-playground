#pragma once
#include <cstdint>

void runJacobiPressureSolve(int n, int m, float *pressure, float *divergence, uint8_t *isSolid, int iterations, float dx);

void runVelocityAdvection(int n, int m, float *horizontalVelocity, float *verticalVelocity, float *nextHorizontalVelocity,
                          float *nextVerticalVelocity, uint8_t *isSolid, float dx, float dy, float dt);
