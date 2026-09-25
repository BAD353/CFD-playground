#pragma once
#include <cstdint>

void runJacobiPressureSolve(int n, int m, double *pressure, double *divergence, uint8_t *isSolid, int iterations, double dx);

void runVelocityAdvection(int n, int m, double *horizontalVelocity, double *verticalVelocity, double *nextHorizontalVelocity,
                          double *nextVerticalVelocity, uint8_t *isSolid, double dx, double dy, double dt);
