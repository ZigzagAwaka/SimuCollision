#include <cmath>
#include <vector>
#include <iostream>
#include "glimac/common.hpp"
#include "glimac/Circle.hpp"


namespace glimac {

    void Circle::build(float radius, int nbP) {
        float angle = 360.0 / float(nbP);
        int nbiTriangles = nbP-2; // number of triangles without the last two

        std::vector<ShapeVertex> data;
        for(int i=0; i<nbP; i++) { // create all vertex
            ShapeVertex vertex;
            
            vertex.texCoords.x = float(i) * (1.0 / float(nbP));
            vertex.texCoords.y = 0.0f;

            vertex.normal.x = cos(glm::radians(angle*float(i)));
            vertex.normal.y = 0.0;
            vertex.normal.z = sin(glm::radians(angle*float(i)));

            vertex.position = radius * vertex.normal;
            
            data.push_back(vertex);
        }

        for(int i=0; i<nbiTriangles; i++) { // create nbiTriangles triangles
            m_Vertices.push_back(data[i]);
            m_Vertices.push_back(data[i+1]);
            m_Vertices.push_back(data[i+2]);
        }
        m_Vertices.push_back(data[nbiTriangles]); // add the last two triangles
        m_Vertices.push_back(data[nbiTriangles+1]);
        m_Vertices.push_back(data[0]);
        m_Vertices.push_back(data[nbiTriangles+1]);
        m_Vertices.push_back(data[0]);
        m_Vertices.push_back(data[1]);

        m_nVertexCount = m_Vertices.size();
    }



    void Circle::buildF(float radius, int nbP, float fatness) {
        float angle = 360.0 / float(nbP);
        int nbiSquares = nbP - 1; // number of squares (2 triangles created)

        std::vector<ShapeVertex> data;
        std::vector<ShapeVertex> dataF;
        for(int i=0; i<nbP; i++) { // create all vertex
            ShapeVertex vertex, vertexF;
            glm::vec3 norm = glm::vec3(cos(glm::radians(angle*float(i))), 0.0, sin(glm::radians(angle*float(i))));
            float texc_x = float(i) * (1.0 / float(nbP));

            vertex.normal = norm;
            vertex.texCoords = glm::vec2(texc_x, 0.0f);
            vertex.position = radius * vertex.normal;

            vertexF.normal = norm;
            vertexF.texCoords = glm::vec2(texc_x, 1.0f);
            vertexF.position = fatness * vertex.normal;
            
            dataF.push_back(vertexF);
            data.push_back(vertex);
        }

        for(int i=0; i<nbiSquares; i++) { // create nbiSquares squares (2*triangles)
            m_Vertices.push_back(data[i]);
            m_Vertices.push_back(data[i+1]);
            m_Vertices.push_back(dataF[i+1]);
            m_Vertices.push_back(dataF[i+1]);
            m_Vertices.push_back(dataF[i]);
            m_Vertices.push_back(data[i]);
        }
        m_Vertices.push_back(data[nbiSquares]); // add the last square
        m_Vertices.push_back(data[0]);
        m_Vertices.push_back(dataF[0]);
        m_Vertices.push_back(dataF[0]);
        m_Vertices.push_back(dataF[nbiSquares]);
        m_Vertices.push_back(data[nbiSquares]);

        m_nVertexCount = m_Vertices.size();
    }

}
