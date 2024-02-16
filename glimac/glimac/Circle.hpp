#pragma once

#include <vector>

#include "common.hpp"


namespace glimac {

/** Load a custom circle.
 * This class is based on the template model of the glimac::Sphere class
 * @param radius radius of the circle
 * @param nbP number of points
 * @param fatness the "fatness" radius of the circle, can be used to turn the circle into a ring
*/
class Circle {
    void build(float radius, int nbP); // in cpp file
    void buildF(float radius, int nbP, float fatness); // in cpp file

public:
    Circle(float radius, int nbP, float fatness):
        m_nVertexCount(0) {
        if(fatness > 0) buildF(radius, nbP, fatness);
        else build(radius, nbP);
    }

    // Renvoit le pointeur vers les données
    const ShapeVertex* getDataPointer() const {
        return &m_Vertices[0];
    }
    
    // Renvoit le nombre de vertex
    int getVertexCount() const {
        return m_nVertexCount;
    }

private:
    std::vector<ShapeVertex> m_Vertices;
    int m_nVertexCount;
};

}