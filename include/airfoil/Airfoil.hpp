#pragma once

#include <math/Vec2.hpp>
#include <field/Field2D.hpp>
#include <simulation/Simulation.hpp>

class Airfoil {
public:
    Airfoil(double maxCamber, double maxCamberPosition, double thickness, double chord, double angleOfAttack, Vec2 position, int width, int height);
    
    const Field2D<CellType>& mesh() const;

    void createNormalizedPolygon();
    void createWorldPolygon();
    void createMesh();

private:
    double m_maxCamber;
    double m_maxCamberPosition;
    double m_thickness;

    double m_chord;
    double m_angleOfAttack;
    Vec2 m_position;

    std::vector<Vec2> m_normalizedPolygon;
    std::vector<Vec2> m_worldPolygon;
    Field2D<CellType> m_mesh;
};