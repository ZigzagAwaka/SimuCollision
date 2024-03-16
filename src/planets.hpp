#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <glimac/Program.hpp>
#include <glimac/FilePath.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/geometric.hpp>
#include <c3ga/Mvec.hpp>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>

#include "c3gaTools.hpp"


struct Planet {
    public:
    int textureIdx; // texture
    float size; // radius
    glm::vec3 position; // x y z
    float obliquity; // rotation of planet axis
    float rotationSpeed; // speed of orbital rotation
    glm::vec3 inclination; // inclination of orbital rotation
    glm::vec3 direction; // where to go
    double spawnTime; // time of spawn
    double durationOfLoad = 0.0; // actual time since spawn
    const double loadPeriod = 1.5; // total time of the loading phase
    bool hasLoaded = false; // to know if the planet is ready to collide
    float visibility = 1.0; // visual factor
    float visibilityOp = -0.1; // visual operation
    unsigned int dirUpdateNb = 0; // update counter for random direction
    static const int dirUpdateRate = 500; // maximum value for dirUpdateNb
    static const int ringSize = 5; // rings global size
    static const int distanceMax = 100; // maximum distance of planets to the center
    static constexpr float minC = 1.5; // minimum possible size of exploding fragments when collision
    static constexpr float explosionRate = 1.07; // the rate at which explosion particles are getting smaller
    static constexpr float explosionSpeed = 0.4; // speed of explosion particles
    static constexpr float explosionMinSize = 0.3; // minimum possible size of explosion particles

    Planet(int t, float s, glm::vec3 p, float o, float rs, glm::vec3 i, double ti) :
        textureIdx{t}, size{s}, position{p}, obliquity{o}, rotationSpeed{rs}, inclination{i}, spawnTime{ti} {
            direction = glm::normalize(glm::sphericalRand(1.0f));
    }

    Planet(const Planet& other) {
        if(this != &other) *this = other;
    }

    Planet& operator=(const Planet& other) {
        if(this != &other) {
            textureIdx = other.textureIdx; size = other.size;
            position = other.position; obliquity = other.obliquity;
            rotationSpeed = other.rotationSpeed; inclination = other.inclination;
            direction = other.direction; spawnTime = other.spawnTime;
            durationOfLoad = other.durationOfLoad; hasLoaded = other.hasLoaded;
            visibility = other.visibility; visibilityOp = other.visibilityOp;
            dirUpdateNb = other.dirUpdateNb;
        }
        return *this;
    }

    // ----- COLLISION DETECTION -----

    private:
    c3ga::Mvec<double> computeSphereCGA() const {
        c3ga::Mvec<double> pt1 = c3ga::point<double>(position.x + size, position.y, position.z);
        c3ga::Mvec<double> pt2 = c3ga::point<double>(position.x - size, position.y, position.z);
        c3ga::Mvec<double> pt3 = c3ga::point<double>(position.x, position.y + size, position.z);
        c3ga::Mvec<double> pt4 = c3ga::point<double>(position.x, position.y, position.z + size);
        return pt1 ^ pt2 ^ pt3 ^ pt4;
    }

    public:
    // return true if the planet has collided with another given planet
    bool hasCollided(Planet other) const {
        auto sphere1 = computeSphereCGA();
        auto sphere2 = other.computeSphereCGA();
        auto circle_d = !sphere1 ^ !sphere2;
        if((double)(circle_d | circle_d) < 0.0) {
            return true;
        }
        return false;
    }

    // ----- RANDOM SELECTION -----

    static int selectTextureIdx() {
        int textureIdxMin = 1;
        int textureIdxMax = 32;
        return selectRandomInt(textureIdxMin, textureIdxMax);
    }

    static float selectSize() {
        int sizeMin = int(Planet::minC * 4.0); // THIS VALUE NEEDS TO ALWAYS BE Planet::minC * 4 !!
        int sizeMax = 20;
        return selectRandomFloat(sizeMin, sizeMax);
    }

    static glm::vec3 selectPosition() {
        int positionDistanceMin = 3;
        int positionDistanceMax = distanceMax;
        return glm::sphericalRand(selectRandomFloat(positionDistanceMin, positionDistanceMax));
    }

    static float selectObliquity() {
        int obliquityMin = 0;
        int obliquityMax = 180;
        return selectRandomFloat(obliquityMin, obliquityMax);
    }

    static float selectRotationSpeed() {
        int lengthOfDaysMin = -500;
        int lengthOfDaysMax = 500;
        float selection = selectRandomFloat(lengthOfDaysMin, lengthOfDaysMax);
        return 1.0 / (selection == 0.0 ? 24.0 : selection);
    }

    static glm::vec3 selectInclination() {
        int inclinationMin = 0;
        int inclinationMax = 20;
        float inc = selectRandomFloat(inclinationMin, inclinationMax);
        if(inc == 0.0) return glm::vec3(0, 1, 0);
        glm::vec4 res = glm::rotate(glm::mat4(1.0), glm::radians(inc), glm::vec3(1, 0, 0))
                        * glm::vec4(glm::vec3(0, 1, 0), 0.0);
        return glm::vec3(glm::normalize(res));
    }

    static int selectExplodingFragments() {
        int nbFragsMin = 4;
        int nbFragsMax = 6;
        return selectRandomInt(nbFragsMin, nbFragsMax);
    }

    private:
    static int selectRandomInt(int min, int max) {
        return rand() % (max - min + 1) + min;
    }

    static float selectRandomFloat(int min, int max) {
        return min + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX/(max-min)));
    }
};



/** Global order for any functions returning multiple planets data (particulary used for textures Id) :
 * sun, mercury, venus, earth, mars, jupiter, saturn, uranus, neptune, pluto,
 * moon, phobos, deimos, calisto, ganymede, europa, io, mimas, enceladus,
 * tethys, dione, rhea, titan, hyperion, iapetus, ariel, umbriel, titania,
 * oberon, miranda, triton, nereid, charon,
 * earthcloud, saturnring, uranusring, skybox, whiteColor
*/



/* Global class containing time and various data  */
struct Info {
    private:
    float f_speed = 510.0; // rotation speed factor
    double time_memory = 0.0; // time of the simulation, if paused
    bool time_pause = false; // flag to know if the time is paused
    bool draw_hitbox = false; // indicator to draw orbit of planets
    bool special_spawn = false; // indicator to spawn a new planet
    bool special_clean = false; // indicator to remove all small planets

    public:
    Info() {}

    /*modify the speed of the simulation*/
    void modifySpeed(float f) {
        float t = f_speed + f;
        if(t < 210.0 || t > 10000.0) return;
        f_speed = t;
    }

    float getFactorSpeed() const {
        return f_speed;
    }

    /*get the rate at which update functions needs to be run*/
    int getUpdateRate() const {
        if(f_speed == 510.0) return 100;
        return int((100.0 * 510.0 / f_speed));
    }

    bool isPaused() const {
        return time_pause;
    }

    /*pause/resume the time of the simulation*/
    void pauseTime() {
        if(time_pause) {
            time_pause = false;
            glfwSetTime(time_memory);
        }
        else {
            time_pause = true;
            time_memory = glfwGetTime();
        }
    }

    /*get the time of the simulation*/
    double getTime() const {
        if(time_pause) return time_memory;
        return glfwGetTime();
    }

    /*to know if we have to draw the orbit or not*/
    bool drawHitbox() const {
        return draw_hitbox;
    }

    /*inverse the draw_orbit flag*/
    void modifyDrawHitbox() {
        draw_hitbox = !draw_hitbox;
    }

    /*to know if we have to activate the special spawn*/
    bool specialSpawn() const {
        return special_spawn;
    }

    /*inverse the special_spawn flag*/
    void modifySpecialSpawn() {
        special_spawn = !special_spawn;
    }

    /*to know if we have to activate the special clean*/
    bool specialClean() const {
        return special_clean;
    }

    /*inverse the special_clean flag*/
    void modifySpecialClean() {
        special_clean = !special_clean;
    }
};



/* Uniform variables (in shaders) */
struct UniformVariables {
    GLint uMVPMatrix; // model view proj
    GLint uMVMatrix; // model view
    GLint uNormalMatrix; // norm
    GLint uTexture0; // texture
    GLint uVisibilityFactor; // brightness
};

/* OpenGl Program of a classic planet */
struct PlanetProgram {
    glimac::Program m_Program;
    UniformVariables u;

    PlanetProgram(const glimac::FilePath& applicationPath):
        m_Program {loadProgram(applicationPath.dirPath() + "src/shaders/position3D.vs.glsl",
                                applicationPath.dirPath() + "src/shaders/tex3D.fs.glsl")} {
        u.uMVPMatrix = glGetUniformLocation(m_Program.getGLId(), "uMVPMatrix");
        u.uMVMatrix = glGetUniformLocation(m_Program.getGLId(), "uMVMatrix");
        u.uNormalMatrix = glGetUniformLocation(m_Program.getGLId(), "uNormalMatrix");
        u.uTexture0 = glGetUniformLocation(m_Program.getGLId(), "uTexture0");
        u.uVisibilityFactor = glGetUniformLocation(m_Program.getGLId(), "uVisibilityFactor");
    };
};
