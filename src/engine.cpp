#include "engine.hpp"


// ============================================================
// TEXTURES
// ============================================================

std::vector<GLuint> createTextureObjects(glimac::FilePath binPath) {
    std::vector<GLuint> textureObjects;
    std::string dir = "assets/textures/";
    std::vector<std::string> textureImages = {
        "sun.jpg", "mercury.jpg", "venus.jpg", "earth.jpg", "mars.jpg", "jupiter.jpg",
        "saturn.jpg", "uranus.jpg", "neptune.jpg", "pluto.jpg", "moon.jpg", "phobos.jpg",
        "deimos.jpg", "calisto.jpg", "ganymede.jpg", "europa.jpg", "io.jpg", "mimas.jpg",
        "enceladus.jpg", "tethys.jpg", "dione.jpg", "rhea.jpg", "titan.jpg", "hyperion.jpg",
        "iapetus.jpg", "ariel.jpg", "umbriel.jpg", "titania.jpg", "oberon.jpg", "miranda.jpg",
        "triton.jpg", "nereid.jpg", "charon.jpg", "earthcloud.jpg", "saturnring.jpg",
        "uranusring.jpg", "skybox.jpg", "white.jpg"};
    for(size_t i=0; i<textureImages.size(); i++) {
        auto image = glimac::loadImage(binPath + dir + textureImages[i]);
        if(image == NULL) {
            std::cerr << "Texture loading " << textureImages[i] << " fail !" << std::endl;
            continue; }
        GLuint texo;
        glGenTextures(1, &texo);
        glBindTexture(GL_TEXTURE_2D, texo);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.get()->getWidth(), image.get()->getHeight(), 0, GL_RGBA, GL_FLOAT, image.get()->getPixels());
        glBindTexture(GL_TEXTURE_2D, 0);
        textureObjects.push_back(texo);
    }
    return textureObjects;
}


// ============================================================
// 3D OBJECTS
// ============================================================

/* load vbo and vao of an object */
void loadModel(GLsizei vertexCount, const glimac::ShapeVertex* dataPointer, GLuint* vbo, GLuint* vao) {
    const GLuint VERTEX_ATTR_POSITION = 0;
    const GLuint VERTEX_ATTR_NORMAL = 1;
    const GLuint VERTEX_ATTR_TEXTURE = 2;
    // VBO
    glGenBuffers(1, vbo);
    glBindBuffer(GL_ARRAY_BUFFER, *vbo);
    glBufferData(GL_ARRAY_BUFFER, vertexCount*sizeof(glimac::ShapeVertex), dataPointer, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    // VAO
    glGenVertexArrays(1, vao);
    glBindVertexArray(*vao);
    glEnableVertexAttribArray(VERTEX_ATTR_POSITION);
    glEnableVertexAttribArray(VERTEX_ATTR_NORMAL);
    glEnableVertexAttribArray(VERTEX_ATTR_TEXTURE);
    glBindBuffer(GL_ARRAY_BUFFER, *vbo);
    glVertexAttribPointer(VERTEX_ATTR_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(glimac::ShapeVertex), (const GLvoid*)offsetof(glimac::ShapeVertex, position));
    glVertexAttribPointer(VERTEX_ATTR_NORMAL, 3, GL_FLOAT, GL_FALSE, sizeof(glimac::ShapeVertex), (const GLvoid*)offsetof(glimac::ShapeVertex, normal));
    glVertexAttribPointer(VERTEX_ATTR_TEXTURE, 2, GL_FLOAT, GL_FALSE, sizeof(glimac::ShapeVertex), (const GLvoid*)offsetof(glimac::ShapeVertex, texCoords));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}


std::vector<Model> createModels(bool lowConfig) {
    std::vector<Model> models; int N = 64;
    if(lowConfig) N = 32;

    glimac::Sphere sphere(1, N, N); // planets, moons, sun
    GLuint vbo0; GLuint vao0;
    loadModel(sphere.getVertexCount(), sphere.getDataPointer(), &vbo0, &vao0);
    Model model0 = Model(vbo0, vao0, sphere.getVertexCount());
    models.push_back(model0);

    glimac::Circle circle(1, N, 0); // orbits
    GLuint vbo1; GLuint vao1;
    loadModel(circle.getVertexCount(), circle.getDataPointer(), &vbo1, &vao1);
    Model model1 = Model(vbo1, vao1, circle.getVertexCount());
    models.push_back(model1);

    glimac::Circle ring(1, N, 1.4); // rings
    GLuint vbo2; GLuint vao2;
    loadModel(ring.getVertexCount(), ring.getDataPointer(), &vbo2, &vao2);
    Model model2 = Model(vbo2, vao2, ring.getVertexCount());
    models.push_back(model2);

    return models;
}


GLuint* getDataOfModels(std::vector<Model> models, int type) {
    std::vector<GLuint> temp;
    for(size_t i=0; i<models.size(); i++) {
        GLuint t;
        if(type == 0) t = models[i].vbo;
        else t = models[i].vao;
        temp.push_back(t);
    }
    return temp.data();
}


// ============================================================
// OPENGL DRAW FUNCTIONS
// ============================================================

// Fill the given uniform variables
// Without light if lightIndicator == 0, with light direction if lightIndicator > 0, or with star time if lightIndicator < 0
void fillUniforms(UniformVariables u, glm::mat4 objectMVMatrix, std::vector<glm::mat4> matrix, float lightIndicator) {
    if(lightIndicator > 0) {
        glUniform3fv(u.uKd, 1, glm::value_ptr(glm::vec3(0.8, 0.7, 0.7)));
        glUniform3fv(u.uKs, 1, glm::value_ptr(glm::vec3(0.5, 0.5, 0.4)));
        glUniform1f(u.uShininess, 3.0f);
        glUniform3fv(u.uLightDir_vs, 1, glm::value_ptr(
            glm::mat3(glm::rotate( glm::rotate(glm::mat4(1.0), glm::radians(180.0f), glm::vec3(0,1,0)), lightIndicator, glm::vec3(0,1,0) ))
            * glm::mat3(matrix[2]) ));
        glUniform3fv(u.uLightIntensity, 1, glm::value_ptr(glm::vec3(0.9, 0.9, 0.88)));
    }
    if(lightIndicator < 0) glUniform1f(u.uTimeSt, -1.0f * lightIndicator);
    glUniformMatrix4fv(u.uMVPMatrix, 1, GL_FALSE, glm::value_ptr(matrix[0] * objectMVMatrix));
    glUniformMatrix4fv(u.uMVMatrix, 1, GL_FALSE, glm::value_ptr(objectMVMatrix));
    glUniformMatrix4fv(u.uNormalMatrix, 1, GL_FALSE, glm::value_ptr(glm::transpose(glm::inverse(objectMVMatrix))));
}

// Activate and bind asked textures (one, or many)
void prepareTextures(int i, UniformVariables u, std::vector<GLuint> textures, bool multiple) {
    glUniform1i(u.uTexture0, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textures[i]);
    if(multiple) {
        glUniform1i(u.uTexture1, 1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, textures[33]); // only earthclouds (index 33), for now
    }
}

// Deactivate and debind multiple layers asked textures
void cleanMultTextures(bool multiple) {
    if(multiple) {
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}

// Draw the skybox
void drawSkybox(ClassicProgram* skybox, std::vector<GLuint> textures, std::vector<Model> models, std::vector<glm::mat4> matrix) {
    float s = 50000.0f;
    glm::mat4 sbMVMatrix = glm::scale(matrix[1], glm::vec3(s, s, s));
    prepareTextures(36, skybox->u, textures, false); // skybox is index 36
    fillUniforms(skybox->u, sbMVMatrix, matrix, 0);
    glDrawArrays(GL_TRIANGLES, 0, models[0].vertexCount);
}

// Draw the n.i asked orbit
void drawOrbit(int i, ClassicProgram* orbit, PlanetInfo info, std::vector<GLuint> textures, std::vector<Model> models, std::vector<glm::mat4> matrix) {
    float d = info.distance(i);
    float orb_inc = info.orbital_inclination(i);
    glm::mat4 orbMVMatrix = glm::rotate(matrix[1], glm::radians(orb_inc), glm::vec3(1, 0, 0));
    orbMVMatrix = glm::scale(orbMVMatrix, glm::vec3(d, d, d));
    prepareTextures(37, orbit->u, textures, false); // color of orbit is basic white (index 37)
    fillUniforms(orbit->u, orbMVMatrix, matrix, 0);
    glDrawArrays(GL_TRIANGLES, 0, models[1].vertexCount);
}

// Draw the ring for the asked planet
void drawRing(int i, PlanetProgram* ring, PlanetInfo info, std::vector<GLuint> textures, std::vector<Model> models, std::vector<glm::mat4> matrix) {
    glBindVertexArray(0); // debind sphere
    glBindVertexArray(models[2].vao); // bind ring
    float d = info.distance(i);
    float s = info.size(i) + info.ringSizeFactor();
    float orb_speed = info.orbital_speed(i);
    float rot_speed = info.rotation_speed(i);
    glm::vec3 axis = info.inclination(i);
    float obli = info.obliquity(i);
    double time = info.getTime();
    glm::mat4 ringMVMatrix = matrix[1];
    if(info.chosenView() != i) { // if the planet is the chosen view, dont apply these 2 transforms
        ringMVMatrix = glm::rotate(ringMVMatrix, float(time * orb_speed), axis);
        ringMVMatrix = glm::translate(ringMVMatrix, glm::vec3(d, 0, 0)); }
    ringMVMatrix = glm::rotate(ringMVMatrix, obli, glm::vec3(1, 0, 0));
    ringMVMatrix = glm::rotate(ringMVMatrix, float(time * rot_speed),  glm::vec3(0, 1, 0));
    ringMVMatrix = glm::scale(ringMVMatrix, glm::vec3(s, s, s));
    prepareTextures(i+28, ring->u, textures, false); // +28 in the global order to get the ring texture of the asked i planet
    fillUniforms(ring->u, ringMVMatrix, matrix, time * orb_speed);
    glDrawArrays(GL_TRIANGLES, 0, models[2].vertexCount);
    glBindVertexArray(0); // debind ring
    glBindVertexArray(models[0].vao); // bind sphere
}

// Draw the sun
void drawSun(StarProgram* star, PlanetInfo info, std::vector<GLuint> textures, std::vector<Model> models, std::vector<glm::mat4> matrix) {
    float s = info.size(0);
    float rot_speed = info.rotation_speed(0);
    double time = info.getTime();
    glm::mat4 sunMVMatrix = glm::rotate(matrix[1], float(time * rot_speed), glm::vec3(0, 1, 0));
    sunMVMatrix = glm::scale(sunMVMatrix, glm::vec3(s, s, s));
    prepareTextures(0, star->u, textures, false); // sun is at index 0
    fillUniforms(star->u, sunMVMatrix, matrix, -1.0f * float(time));
    glDrawArrays(GL_TRIANGLES, 0, models[0].vertexCount);
}

// Draw the n.i asked planet
void drawPlanet(int i, PlanetProgram* planet, PlanetInfo info, std::vector<GLuint> textures, std::vector<Model> models, std::vector<glm::mat4> matrix) {
    float d = info.distance(i);
    float s = info.size(i);
    float orb_speed = info.orbital_speed(i);
    float rot_speed = info.rotation_speed(i);
    glm::vec3 axis = info.inclination(i);
    float obli = info.obliquity(i);
    double time = info.getTime();
    bool mult = info.hasMultipleTex(i);
    glm::mat4 planetMVMatrix = matrix[1];
    if(info.chosenView() != i) { // if the planet is the chosen view, dont apply these 2 transforms
        planetMVMatrix = glm::rotate(planetMVMatrix, float(time * orb_speed), axis);
        planetMVMatrix = glm::translate(planetMVMatrix, glm::vec3(d, 0, 0)); }
    planetMVMatrix = glm::rotate(planetMVMatrix, obli, glm::vec3(1, 0, 0));
    planetMVMatrix = glm::rotate(planetMVMatrix, float(time * rot_speed), axis);
    planetMVMatrix = glm::scale(planetMVMatrix, glm::vec3(s, s, s));
    prepareTextures(i, planet->u, textures, mult);
    fillUniforms(planet->u, planetMVMatrix, matrix, time * orb_speed);
    glDrawArrays(GL_TRIANGLES, 0, models[0].vertexCount);
    cleanMultTextures(mult); // clean cloud texture for Earth
    if(info.hasRings(i)) drawRing(i, planet, info, textures, models, matrix); // draw ring if applicable
}


void drawEverything(StarProgram* star, PlanetProgram* planet, ClassicProgram* classicObj, PlanetInfo info, std::vector<GLuint> textures, std::vector<Model> models, std::vector<glm::mat4> matrix) {
    int view = info.chosenView(); // get chosen view
    classicObj->m_Program.use();
    if(info.drawOrbit()) {
        glBindVertexArray(models[1].vao); // bind circle
        for(int i=info.orbitBegin(view); i<info.orbitEnd(view); i++) { // draw orbits
            drawOrbit(i, classicObj, info, textures, models, matrix);
        }
        glBindVertexArray(0);
    }
    glBindVertexArray(models[0].vao); // bind sphere
    drawSkybox(classicObj, textures, models, matrix);
    if(view == 0) { // draw the sun at view 0
        star->m_Program.use();
        drawSun(star, info, textures, models, matrix);
    }
    planet->m_Program.use();
    if(view != 0) { // draw the chosen planet at view != 0
        drawPlanet(view, planet, info, textures, models, matrix);
    }
    for(int i=info.orbitBegin(view); i<info.orbitEnd(view); i++) { // draw planets/moons in orbit
        drawPlanet(i, planet, info, textures, models, matrix);
    }
    glBindVertexArray(0);
}
