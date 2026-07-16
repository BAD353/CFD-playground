#pragma once

#include "field/Field2D.hpp"
#include "math/Vec2.hpp"
#include "mesh/Grid.hpp"

enum class CellType {
    Fluid, 
    Solid
};

class Simulation {
  public:
    Simulation(const Grid &grid);

    void update(double dt);

    const Field2D<double> &horizontalVelocity() const;
    const Field2D<double> &verticalVelocity() const;
    const Field2D<double> &divergence() const;
    const Field2D<double> &dye() const;
    const Field2D<double> &pressure() const;
    const Field2D<CellType> &cellType() const;

    double sampleField(double x, double y, const Field2D<double> &field) const;
    double sampleHorizontalVelocity(double worldX, double worldY) const;
    double sampleVerticalVelocity(double worldX, double worldY) const;
    double sampleDye(double worldX, double worldY) const;
    
    
    void initNextVelocity();
    void initZeroPressure();
    void initNextPressure();

    void applyGravityForce(double dt);
    void applyMouseForce(Vec2 position, Vec2 force, double radius, double dt);
    void updateCellType(Vec2 position, CellType cellType, double radius);

    void applyAdvection(double dt);

    void compute_divergence();
    double find_max_divergence();

    void applyPressureJacobiIteration();
    void applyViscosityJacobiIteration(double dt);

    void applyPressureGradient();

    void initializeVelocity();

    void applyVelocityBoundary();
    void applyPressureBoundary();

    void injectDye(Vec2 position, double radius, double dt);
    void applyDyeAdvection(double dt);

    bool isHorizontalFaceSolid(int x, int y) const;
    bool isVerticalFaceSolid(int x, int y) const;

  private:
    Grid m_grid;
    double m_alpha;

    const double viscosity = 5;
    
    // Old velocity fields
    // Field2D<Vec2> m_velocity0;
    // Field2D<Vec2> m_velocity;
    // Field2D<Vec2> m_nextVelocity;

    Field2D<double> m_horizontalVelocity0;
    Field2D<double> m_horizontalVelocity;
    Field2D<double> m_nextHorizontalVelocity;

    Field2D<double> m_verticalVelocity0;
    Field2D<double> m_verticalVelocity;
    Field2D<double> m_nextVerticalVelocity;


    Field2D<double> m_pressure;
    Field2D<double> m_nextPressure;

    Field2D<double> m_divergence;

    Field2D<double> m_dye;
    Field2D<double> m_nextDye;

    Field2D<CellType> m_cellType;
};