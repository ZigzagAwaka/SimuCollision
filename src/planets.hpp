#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <glimac/Program.hpp>
#include <glimac/FilePath.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/geometric.hpp>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>


/** Global order for any functions returning multiple planets data :
 * sun, mercury, venus, earth, mars, jupiter, saturn, uranus, neptune, pluto,
 * moon, phobos, deimos, calisto, ganymede, europa, io, mimas, enceladus,
 * tethys, dione, rhea, titan, hyperion, iapetus, ariel, umbriel, titania,
 * oberon, miranda, triton, nereid, charon,
 * earthcloud, saturnring, uranusring, skybox, whiteColor
*/


/* Indexes of all objects located in the searched orbit following the above global order.
    The following list of index defines a range to every objects having other objects
    turning around them, for example:
    - the Earth (3rd planet in the solar system, so at index 3 in the vector) only has
      the Moon so the index begin at 10 (== the Moon in the global order) and end
      at 10 (== also the Moon)
    - Jupiter (index 5) has 4 moons from Calisto to Io (from index 13 to 16 in the
      global order)
    - Mercury does not have anything so in that case the indexes are set from 0 to -1
      (will not draw any moons)*/
struct OrbitIndexs {
    int chosenView = 0; // actual view of the simulation, 0=sun, 1=mercury, ...
    std::vector<int> begin = {1,  0,  0, 10, 11, 13, 17, 25, 30, 32};
    std::vector<int> end   = {9, -1, -1, 10, 12, 16, 24, 29, 31, 32};
};

/* Parameters of planets */
struct PlanetParams {
    float perihelion;
    float aphelion;
    float diameter;
    float orbital_period;
    float length_of_days;
    float orbital_inclination;
    float obliquity;
};


/* Global structure to get every parameters of the planets */
struct PlanetInfo {
    private:
    std::vector<PlanetParams> p; // planets parameters
    float size_fix = 20.0; // modify size of planets
    float size_fix_v = 170.0; // modify size of planets when view != 0
    float size_fix_v_orbit = 240.0; // modify size of moons when view != 0
    float f_size_ring = 170.0; // rings size factor
    float sun_size_fix = 2.0; // modify size of sun
    float distance_offset = 150.0; // because of sun_size_fix, some planets are too close so little correction
    float distance_offset_v = 2700.0; // because of size_fix_v, some moons are too close so little correction
    float f_distance = 3.2; // distance factor
    float f_distance_v = 1500.0; // distance factor when view != 0
    float f_speed = 510.0; // orbit and rotation speed
    double time_memory = 0.0; // time of the simulation, if paused
    bool time_pause = false; // flag to know if the time is paused
    bool draw_orbit = true; // indicator to draw orbit of planets
    glm::vec3 ecliptic = glm::vec3(0, 1, 0); // rotation axis of earth
    OrbitIndexs oi; // indexes in orbit

    public:
    PlanetInfo() {
        p = {
            PlanetParams{0.0, 0.0, 1391400.0, 0.0, 25.0, 0.0, 0.0}, // sun
            PlanetParams{46.0, 69.8, 4879.0, 88.0, 4222.6, 7.0, 0.0}, // mercury
            PlanetParams{107.5, 108.9, 12104.0, 224.7, -2802.0, 3.4, 177.4}, // venus
            PlanetParams{147.1, 152.1, 12756.0, 365.2, 24.0, 0.0, 23.4}, // earth
            PlanetParams{206.7, 249.3, 6792.0, 687.0, 24.7, 1.8, 25.2}, // mars
            PlanetParams{740.6, 816.4, 142984.0, 4331.0, 9.9, 1.3, 3.1}, // jupiter
            PlanetParams{1357.6, 1506.5, 120536.0, 10747.0, 10.7, 2.5, 26.7}, // saturn
            PlanetParams{2732.7, 3001.4, 51118.0, 30589.0, -17.2, 0.8, 97.8}, // uranus
            PlanetParams{4471.1, 4558.9, 49528.0, 59800.0, 16.1, 1.8, 28.3}, // neptune
            PlanetParams{4436.8, 7375.9, 2376.0, 90560.0, 153.3, 17.2, 122.5}, // pluto
            PlanetParams{0.3, 0.3, 3476.2, 27.3, 27.3, 5.1, 6.7}, // moon
            PlanetParams{0.009, 0.009, 11.0, 0.3, 0.3, 1.0, 0.0}, // phobos
            PlanetParams{0.02, 0.02, 6.0, 1.2, 1.2, 1.7, 0.0}, // deimos
            PlanetParams{1.8, 1.8, 4820.6, 16.6, 16.6, 0.1, 0.0}, // calisto
            PlanetParams{1.0, 1.0, 5262.4, 7.1, 7.1, 0.1, 0.0}, // ganymede
            PlanetParams{0.6, 0.6, 3121.6, 3.5, 3.5, 0.4, 0.0}, // europa
            PlanetParams{0.4, 0.4, 3643.0, 1.7, 1.7, 0.0, 0.0}, // io
            PlanetParams{0.1, 0.1, 416.0, 0.9, 0.9, 1.5, 0.0}, // mimas
            PlanetParams{0.2, 0.2, 514.0, 1.3, 1.3, 0.0, 0.0}, // enceladus
            PlanetParams{0.2, 0.2, 1076.0, 1.8, 1.8, 1.8, 0.0}, // tethys
            PlanetParams{0.3, 0.3, 1126.0, 2.7, 2.7, 0.0, 0.0}, // dione
            PlanetParams{0.5, 0.5, 1530.0, 4.5, 4.5, 0.3, 0.0}, // rhea
            PlanetParams{1.2, 1.2, 5150.0, 15.9, 15.9, 0.3, 0.0}, // titan
            PlanetParams{1.5, 1.5, 360.0, 21.2, 21.2, 0.4, 0.0}, // hyperion
            PlanetParams{3.5, 3.5, 1492.0, 79.3, 79.3, 14.7, 0.0}, // iapetus
            PlanetParams{0.1, 0.1, 1162.2, 2.5, 2.5, 0.0, 0.0}, // ariel
            PlanetParams{0.2, 0.2, 1169.4, 4.1, 4.1, 0.1, 0.0}, // umbriel
            PlanetParams{0.4, 0.4, 1577.8, 8.7, 8.7, 0.0, 0.0}, // titania
            PlanetParams{0.5, 0.5, 1522.8, 13.4, 13.4, 0.0, 0.0}, // oberon
            PlanetParams{0.1, 0.1, 480.0, 1.4, 1.4, 4.3, 0.0}, // miranda
            PlanetParams{0.3, 0.3, 2706.8, 5.8, 5.8, 157.3, 0.0}, // triton
            PlanetParams{5.5, 5.5, 340.0, 360.1, 360.1, 7.2, 0.0}, // nereid
            PlanetParams{0.01, 0.01, 1212.0, 6.3, 6.3, 0.0, 0.0} // charon
        };
    }

    /*closest distance to the sun/planet*/
    float perihelion(int i) {
        if(chosenView() != 0) return p[i].perihelion * f_distance_v + distance_offset_v; // moons
        return p[i].perihelion * f_distance + distance_offset; // planets
    }

    /*furthest distance to the sun/planet*/
    float aphelion(int i) {
        if(chosenView() != 0) return p[i].aphelion * f_distance_v + distance_offset_v; // moons
        return p[i].aphelion * f_distance + distance_offset; // planets
    }

    /*average distance to the sun/planet*/
    float distance(int i) {
        return (perihelion(i) + aphelion(i)) / 2.0;
    }
    
    /*diameter of the planet*/
    float diameter(int i) {
        return p[i].diameter;
    }

    /*ratio size of the planet compared to earth*/
    float size(int i) {
        float d = diameter(i) / diameter(3); // initial size
        if(i == 0) return d * sun_size_fix; // sun
        if(chosenView() != 0) {
            if(i >= 10) return d * size_fix_v_orbit; // moons (global order>=10) in orbit when view != 0
            return d * size_fix_v; // planets when view != 0
        }
        return d * size_fix; // planets around the sun
    }
    
    /*number of days for an orbit around the sun/planet*/
    float orbital_period(int i) {
        return p[i].orbital_period;
    }

    /*speed for a full orbit around the sun/planet*/
    float orbital_speed(int i) {
        return 1.0 / orbital_period(i) * f_speed;
    }
    
    /*time for the planet to have a proper orbit*/
    float length_of_days(int i) {
        return p[i].length_of_days;
    }

    /*speed for a proper orbit of the planet*/
    float rotation_speed(int i) {
        return 1.0 / length_of_days(i) * f_speed;
    }
    
    /*inclination of the orbit, in degrees*/
    float orbital_inclination(int i) {
        return p[i].orbital_inclination;
    }

    /*inclination of the orbit, given by a vec3*/
    glm::vec3 inclination(int i) {
        float inc = orbital_inclination(i);
        if(inc == 0.0) return ecliptic;
        glm::vec4 res = glm::rotate(glm::mat4(1.0), glm::radians(inc), glm::vec3(1, 0, 0))
                        * glm::vec4(ecliptic, 0.0);
        return glm::vec3(glm::normalize(res));
    }

    /*get the size factor of ring*/
    float ringSizeFactor() {
        return f_size_ring;
    }

    /*inclination of the planet compared to the orbit, in radians*/
    float obliquity(int i) {
        return glm::radians(p[i].obliquity);
    }

    /*modify the speed of the simulation*/
    void modifySpeed(float f) {
        float t = f_speed + f;
        if(t < 10.0 || t > 10000.0) return;
        f_speed = t;
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
    double getTime() {
        if(time_pause) return time_memory;
        return glfwGetTime();
    }

    /*to know if we have to draw the orbit or not*/
    bool drawOrbit() {
        return draw_orbit;
    }

    /*inverse the draw_orbit flag*/
    void modifyDrawOrbit() {
        draw_orbit = !draw_orbit;
    }

    /*return the actual chosen view of the drawing*/
    int chosenView() {
        return oi.chosenView;
    }

    /*modify the actual chosen view*/
    void modifyChosenView() {
        int actual = chosenView();
        if(actual == 9) oi.chosenView = 0;
        else oi.chosenView = actual + 1;
    }

    /*the begin orbit index of the asked planet*/
    int orbitBegin(int i) {
        return oi.begin[i];
    }

    /*the end orbit index of the asked planet*/
    int orbitEnd(int i) {
        return oi.end[i] + 1;
    }

    /*return true if the asked planet has a ring system*/
    bool hasRings(int i) {
        return i == 6 || i == 7; // only Saturn and Uranus
    }

    /*return true if the asked planet has multiple texture layers*/
    bool hasMultipleTex(int i) {
        return i == 3; // only the Earth, for now
    }
};



/* Uniform variables (in shaders) */
struct UniformVariables {
    GLint uMVPMatrix; // model view proj
    GLint uMVMatrix; // model view
    GLint uNormalMatrix; // norm
    GLint uTexture0;
    GLint uTexture1;
    GLint uKd; // material diffuse reflection
    GLint uKs; // material glossy reflection
    GLint uShininess; // material shine factor
    GLint uLightDir_vs; // light direction
    GLint uLightIntensity;
    GLint uTimeSt; // time passed for the star
};

/* Program of a classic planet, or celestial body (can receive light) */
struct PlanetProgram {
    glimac::Program m_Program;
    UniformVariables u;

    PlanetProgram(const glimac::FilePath& applicationPath):
        m_Program {loadProgram(applicationPath.dirPath() + "src/shaders/position3D.vs.glsl",
                                applicationPath.dirPath() + "src/shaders/multiTex3D_light.fs.glsl")} {
        u.uMVPMatrix = glGetUniformLocation(m_Program.getGLId(), "uMVPMatrix");
        u.uMVMatrix = glGetUniformLocation(m_Program.getGLId(), "uMVMatrix");
        u.uNormalMatrix = glGetUniformLocation(m_Program.getGLId(), "uNormalMatrix");
        u.uTexture0 = glGetUniformLocation(m_Program.getGLId(), "uTexture0");
        u.uTexture1 = glGetUniformLocation(m_Program.getGLId(), "uTexture1");
        u.uKd = glGetUniformLocation(m_Program.getGLId(), "uKd");
        u.uKs = glGetUniformLocation(m_Program.getGLId(), "uKs");
        u.uShininess = glGetUniformLocation(m_Program.getGLId(), "uShininess");
        u.uLightDir_vs = glGetUniformLocation(m_Program.getGLId(), "uLightDir_vs");
        u.uLightIntensity = glGetUniformLocation(m_Program.getGLId(), "uLightIntensity");
    };
};

/* Program of the sun (create light) */
struct StarProgram {
    glimac::Program m_Program;
    UniformVariables u;

    StarProgram(const glimac::FilePath& applicationPath):
        m_Program {loadProgram(applicationPath.dirPath() + "src/shaders/position3D.vs.glsl",
                                applicationPath.dirPath() + "src/shaders/tex3D_star.fs.glsl")} {
        u.uMVPMatrix = glGetUniformLocation(m_Program.getGLId(), "uMVPMatrix");
        u.uMVMatrix = glGetUniformLocation(m_Program.getGLId(), "uMVMatrix");
        u.uNormalMatrix = glGetUniformLocation(m_Program.getGLId(), "uNormalMatrix");
        u.uTexture0 = glGetUniformLocation(m_Program.getGLId(), "uTexture0");
        u.uTimeSt = glGetUniformLocation(m_Program.getGLId(), "uTimeSt");
    };
};

/* Program of the skybox, or any other basic object (without light) */
struct ClassicProgram {
    glimac::Program m_Program;
    UniformVariables u;

    ClassicProgram(const glimac::FilePath& applicationPath):
        m_Program {loadProgram(applicationPath.dirPath() + "src/shaders/position3D.vs.glsl",
                                applicationPath.dirPath() + "src/shaders/tex3D.fs.glsl")} {
        u.uMVPMatrix = glGetUniformLocation(m_Program.getGLId(), "uMVPMatrix");
        u.uMVMatrix = glGetUniformLocation(m_Program.getGLId(), "uMVMatrix");
        u.uNormalMatrix = glGetUniformLocation(m_Program.getGLId(), "uNormalMatrix");
        u.uTexture0 = glGetUniformLocation(m_Program.getGLId(), "uTexture0");
    };
};