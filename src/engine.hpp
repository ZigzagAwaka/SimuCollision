#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <glimac/Program.hpp>
#include <glimac/Image.hpp>
#include <glimac/FilePath.hpp>
#include <glimac/Sphere.hpp>
#include <glimac/Circle.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/geometric.hpp>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <set>

#include "camera.hpp"
#include "planets.hpp"


/* structure used to represent a 3D model (vbo + vao + vertexCount) */
struct Model {
    GLuint vbo;
    GLuint vao;
    GLsizei vertexCount;

    Model(GLuint m_vbo, GLuint m_vao, GLsizei m_vc) {
        vbo = m_vbo;
        vao = m_vao;
        vertexCount = m_vc;
    }
};


/** Load every textures. The returned vector contains all textures in the global order
 * @param binPath the path to the executable
 * @return a vector of GLuint, or every created textures */
std::vector<GLuint> createTextureObjects(glimac::FilePath binPath);

/** Load every needed models (3D objects).
 * @param lowConfig true if need to load less detailed models or false otherwise
 * @return a vector containing all models at the given indexes
 * 0=sphere, 1=circle, 2=ring */
std::vector<Model> createModels(bool lowConfig);

/** At the end of the program, we need to clean every VBO and VAO using
 * glDeleteBuffers() and glDeleteVertexArrays(). But the VBOs and VAOs are
 * stored in a vector of Models, thus this function returns a fresh
 * pointer to the allocated data of VBOs or VAOs
 * @param models vector of loaded Models
 * @param type 0 for returning VBOs data, or 1 for VAOs
 * @return a pointer in memory to the allocated data of VBO or VAO */
GLuint* getDataOfModels(std::vector<Model> models, int type);

/**Create the initial planet vector*/
std::vector<Planet> createAllPlanets(int nb, double actualTime);

/** Draw every objects for the simulation
 * @param planets vector containing every planets
 * @param explosions vector containing every explosions (particles)
 * @param planet opengl program structure of planets
 * @param info Info structure containing various data, including time
 * @param textures vector containing every pre-loaded textures
 * @param models vector containing every pre-loaded models (sphere, circle, ...)
 * @param matrix vector containing the ProjMatrix, globalMVMatrix and viewMatrix */
void drawEverything(std::vector<Planet> planets, std::vector<Planet> explosions, PlanetProgram* planet,
    Info info, std::vector<GLuint> textures, std::vector<Model> models, std::vector<glm::mat4> matrix);

/**Update every planets parameters*/
void updateEverything(std::vector<Planet>* planets, std::vector<Planet>* explosions, Info* info);

/**Update the visibility (brightness) of planets when they are not loaded*/
void updateVisibility(std::vector<Planet>* planets, Info info);