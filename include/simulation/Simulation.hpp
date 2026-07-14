#pragma once

#include "field/Field2D.hpp"
#include "math/Vec2.hpp"
#include "mesh/Grid.hpp"

class Simulation {
  public:
    Simulation(const Grid &grid);

    void update(double dt);

    const Field2D<Vec2> &velocity() const;

    Vec2 sampleVelocity(double x, double y) const;

    void initNextVelocity();
    void initZeroPressure();
    void initNextPressure();

    void applyGravityForce(double dt);
    void applyMouseForce(Vec2 position, Vec2 force, double radius, double dt);

    void applyAdvection(double dt);

    const Field2D<double> &divergence() const;
    void compute_divergence();
    double find_max_divergence();

    void applyPressureJacobiIteration();
    void applyViscosityJacobiIteration(double dt);

    void applyPressureGradient();

    void initializeVelocity();

    void applyVelocityBoundary();
    void applyPressureBoundary();

    const Field2D<double> &dye() const;
    void injectDye(Vec2 position, double radius, double dt);
    double sampleDye(double x, double y) const;
    void applyDyeAdvection(double dt);

    const Field2D<double> &pressure() const;

  private:
    Grid m_grid;
    double m_alpha;

    const double viscosity = 5;

    Field2D<Vec2> m_velocity0;
    Field2D<Vec2> m_velocity;
    Field2D<Vec2> m_nextVelocity;

    Field2D<double> m_pressure;
    Field2D<double> m_nextPressure;

    Field2D<double> m_divergence;

    Field2D<double> m_dye;
    Field2D<double> m_nextDye;
};