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


/**Create one planet from the parameters, if given, or else select random parameters*/
Planet createPlanet(double actualTime, int size = Planet::selectSize(), glm::vec3 position = Planet::selectPosition(),
                    int textureIdx = Planet::selectTextureIdx(), float obliquity = Planet::selectObliquity(),
                    float rotationSpeed = Planet::selectRotationSpeed(), glm::vec3 inclination = Planet::selectInclination()) {
    return Planet(textureIdx, size, position, obliquity, rotationSpeed, inclination, actualTime);
}

std::vector<Planet> createAllPlanets(int nb, double actualTime) {
    std::vector<Planet> planets;
    for(int n=0; n<nb; n++) {
        planets.push_back(createPlanet(actualTime));
    }
    return planets;
}

/**Add a new explosion in the explosions vector*/
void addExplosion(std::vector<Planet>* explosions, double actualTime, int size, glm::vec3 position) {
    int NB = Planet::selectExplodingFragments();
    for(int n=0; n<NB; n++) {
        explosions->push_back(createPlanet(actualTime, size, position, 37));
    }
}


// ============================================================
// OPENGL FUNCTIONS
// ============================================================

// Fill the given uniform variables
// Without light if lightIndicator == 0, with light direction if lightIndicator > 0, or with star time if lightIndicator < 0
void fillUniforms(UniformVariables u, glm::mat4 objectMVMatrix, std::vector<glm::mat4> matrix, float visibility) {
    // if(lightIndicator > 0) {
    //     glUniform3fv(u.uKd, 1, glm::value_ptr(glm::vec3(0.8, 0.7, 0.7)));
    //     glUniform3fv(u.uKs, 1, glm::value_ptr(glm::vec3(0.5, 0.5, 0.4)));
    //     glUniform1f(u.uShininess, 3.0f);
    //     glUniform3fv(u.uLightDir_vs, 1, glm::value_ptr(
    //         glm::mat3(glm::rotate( glm::rotate(glm::mat4(1.0), glm::radians(180.0f), glm::vec3(0,1,0)), lightIndicator, glm::vec3(0,1,0) ))
    //         * glm::mat3(matrix[2]) ));
    //     glUniform3fv(u.uLightIntensity, 1, glm::value_ptr(glm::vec3(0.9, 0.9, 0.88)));
    // }
    // if(lightIndicator < 0) glUniform1f(u.uTimeSt, -1.0f * lightIndicator);
    glUniformMatrix4fv(u.uMVPMatrix, 1, GL_FALSE, glm::value_ptr(matrix[0] * objectMVMatrix));
    glUniformMatrix4fv(u.uMVMatrix, 1, GL_FALSE, glm::value_ptr(objectMVMatrix));
    glUniformMatrix4fv(u.uNormalMatrix, 1, GL_FALSE, glm::value_ptr(glm::transpose(glm::inverse(objectMVMatrix))));
    glUniform1f(u.uVisibilityFactor, visibility);
}

// Activate and bind the asked texture
void prepareTextures(int id, UniformVariables u, std::vector<GLuint> textures) {
    glUniform1i(u.uTexture0, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textures[id]);
    // if(multiple) {
    //     glUniform1i(u.uTexture1, 1);
    //     glActiveTexture(GL_TEXTURE1);
    //     glBindTexture(GL_TEXTURE_2D, textures[33]); // only earthclouds (index 33), for now
    // }
}

// Deactivate and debind multiple layers asked textures
// void cleanMultTextures(bool multiple) {
//     if(multiple) {
//         glActiveTexture(GL_TEXTURE1);
//         glBindTexture(GL_TEXTURE_2D, 0);
//     }
// }


// ============================================================
// DRAW FUNCTIONS
// ============================================================

// Draw the skybox
void drawSkybox(PlanetProgram* skybox, std::vector<GLuint> textures, std::vector<Model> models, std::vector<glm::mat4> matrix) {
    float s = 5000.0f;
    glm::mat4 sbMVMatrix = glm::scale(matrix[1], glm::vec3(s, s, s));
    prepareTextures(36, skybox->u, textures); // skybox is index 36
    fillUniforms(skybox->u, sbMVMatrix, matrix, 1);
    glDrawArrays(GL_TRIANGLES, 0, models[0].vertexCount);
}

// Draw the global planets hitbox
void drawHitbox(PlanetProgram* program, std::vector<GLuint> textures, std::vector<Model> models, std::vector<glm::mat4> matrix) {
    int size = Planet::distanceMax;
    // float d = info.distance(i);
    // float orb_inc = info.orbital_inclination(i);
    glm::mat4 orbMVMatrix = /*glm::rotate(matrix[1], glm::radians(orb_inc), glm::vec3(1, 0, 0));
    orbMVMatrix =*/ glm::scale(matrix[1], glm::vec3(size, size, size));
    prepareTextures(37, program->u, textures); // color of orbit is basic white (index 37)
    fillUniforms(program->u, orbMVMatrix, matrix, 1);
    glDrawArrays(GL_POINTS, 0, models[0].vertexCount);
}

// Draw the ring for the asked planet
void drawRing(Planet planet, PlanetProgram* program, Info info, std::vector<GLuint> textures, std::vector<Model> models, std::vector<glm::mat4> matrix) {
    glBindVertexArray(0); // debind sphere
    glBindVertexArray(models[2].vao); // bind ring
    glm::vec3 p = /*info.distance(i)*/ planet.position;
    float s = /*info.size(i) + info.ringSizeFactor()*/ planet.size + Planet::ringSize;
    // float orb_speed = info.orbital_speed(i);
    // float rot_speed = info.rotation_speed(i);
    // glm::vec3 axis = info.inclination(i);
    // float obli = info.obliquity(i);
    double time = info.getTime();
    glm::mat4 ringMVMatrix = matrix[1];
    // if(info.chosenView() != i) { // if the planet is the chosen view, dont apply these 2 transforms
        // ringMVMatrix = glm::rotate(ringMVMatrix, float(time * orb_speed), axis);
    ringMVMatrix = glm::translate(ringMVMatrix, /*glm::vec3(d, 0, 0)*/p);
    // }
    ringMVMatrix = glm::rotate(ringMVMatrix, /*obli*/ planet.obliquity, glm::vec3(1, 0, 0));
    ringMVMatrix = glm::rotate(ringMVMatrix, float(time * /*rot_speed*/ (planet.rotationSpeed * info.getFactorSpeed())), planet.inclination /*glm::vec3(0, 1, 0)*/);
    ringMVMatrix = glm::scale(ringMVMatrix, glm::vec3(s, s, s));
    prepareTextures(planet.textureIdx+28, program->u, textures); // +28 in the global order to get the ring texture of the asked i planet
    fillUniforms(program->u, ringMVMatrix, matrix, /*time * orb_speed*/ planet.visibility);
    glDrawArrays(GL_TRIANGLES, 0, models[2].vertexCount);
    glBindVertexArray(0); // debind ring
    glBindVertexArray(models[0].vao); // bind sphere
}

// Draw the sun
// void drawSun(PlanetProgram* star, Info info, std::vector<GLuint> textures, std::vector<Model> models, std::vector<glm::mat4> matrix) {
//     float s = info.size(0);
//     float rot_speed = info.rotation_speed(0);
//     double time = info.getTime();
//     glm::mat4 sunMVMatrix = glm::rotate(matrix[1], float(time * rot_speed), glm::vec3(0, 1, 0));
//     sunMVMatrix = glm::scale(sunMVMatrix, glm::vec3(s, s, s));
//     prepareTextures(0, star->u, textures, false); // sun is at index 0
//     fillUniforms(star->u, sunMVMatrix, matrix, -1.0f * float(time), 1);
//     glDrawArrays(GL_TRIANGLES, 0, models[0].vertexCount);
// }

// Draw the n.i asked explosion
void drawExplosion(Planet explosion, PlanetProgram* program, std::vector<GLuint> textures, std::vector<Model> models, std::vector<glm::mat4> matrix) {
    glm::vec3 p = /*info.distance(i);*/ explosion.position;
    float s = /*info.size(i);*/ explosion.size;
    // float orb_speed = info.orbital_speed(i);
    // float rot_speed = info.rotation_speed(i);
    // glm::vec3 axis = info.inclination(i);
    // float obli = info.obliquity(i);
    // double time = info.getTime();
    // bool mult = info.hasMultipleTex(i);
    glm::mat4 explosionMVMatrix = matrix[1];
    explosionMVMatrix = glm::translate(explosionMVMatrix, p);
    explosionMVMatrix = glm::scale(explosionMVMatrix, glm::vec3(s, s, s));
    prepareTextures(explosion.textureIdx, program->u, textures);
    fillUniforms(program->u, explosionMVMatrix, matrix, 1.0);
    glDrawArrays(GL_TRIANGLES, 0, models[0].vertexCount);
}

// Draw the n.i asked planet
void drawPlanet(Planet planet, PlanetProgram* program, Info info, std::vector<GLuint> textures, std::vector<Model> models, std::vector<glm::mat4> matrix) {
    glm::vec3 p = /*info.distance(i);*/ planet.position;
    float s = /*info.size(i);*/ planet.size;
    // float orb_speed = info.orbital_speed(i);
    // float rot_speed = info.rotation_speed(i);
    // glm::vec3 axis = info.inclination(i);
    // float obli = info.obliquity(i);
    double time = info.getTime();
    // bool mult = info.hasMultipleTex(i);
    glm::mat4 planetMVMatrix = matrix[1];
    //if(info.chosenView() != i) { // if the planet is the chosen view, dont apply these 2 transforms
    // planetMVMatrix = glm::rotate(planetMVMatrix, float(time * orb_speed), axis);
    planetMVMatrix = glm::translate(planetMVMatrix, /*glm::vec3(d, 0, 0)*/ p);
    //}
    planetMVMatrix = glm::rotate(planetMVMatrix, /*obli*/ planet.obliquity, glm::vec3(1, 0, 0));
    // planetMVMatrix = glm::rotate(planetMVMatrix, float(time * rot_speed), axis);
    planetMVMatrix = glm::rotate(planetMVMatrix, float(time * (planet.rotationSpeed * info.getFactorSpeed())), planet.inclination);
    planetMVMatrix = glm::scale(planetMVMatrix, glm::vec3(s, s, s));
    // prepareTextures(i, program->u, textures, mult);
    prepareTextures(planet.textureIdx, program->u, textures);
    fillUniforms(program->u, planetMVMatrix, matrix, /*time * orb_speed*/ planet.visibility);
    glDrawArrays(GL_TRIANGLES, 0, models[0].vertexCount);
    // cleanMultTextures(mult); // clean cloud texture for Earth
    if(/*info.hasRings(i)*/ planet.textureIdx == 6 || planet.textureIdx == 7) drawRing(planet, program, info, textures, models, matrix); // draw ring if applicable
}


void drawEverything(/*StarProgram* star,*/std::vector<Planet> planets, std::vector<Planet> explosions, PlanetProgram* program, /*ClassicProgram* classicObj,*/ Info info, std::vector<GLuint> textures, std::vector<Model> models, std::vector<glm::mat4> matrix) {
    // int view = info.chosenView(); // get chosen view
    program->m_Program.use();
    // if(info.drawOrbit()) {
    //     glBindVertexArray(models[1].vao); // bind circle
    //     for(int i=info.orbitBegin(view); i<info.orbitEnd(view); i++) { // draw orbits
    //         drawOrbit(i, classicObj, info, textures, models, matrix);
    //     }
    //     glBindVertexArray(0);
    // }
    glBindVertexArray(models[0].vao); // bind sphere
    drawSkybox(program, textures, models, matrix);
    if(info.drawHitbox()) drawHitbox(program, textures, models, matrix);
    // if(view == 0) { // draw the sun at view 0
    //     star->m_Program.use();
    //     drawSun(star, info, textures, models, matrix);
    // }
    // planet->m_Program.use();
    // if(view != 0) { // draw the chosen planet at view != 0
    //     drawPlanet(view, planet, info, textures, models, matrix);
    // }
    //for(int i=info.orbitBegin(0); i<info.orbitEnd(0); i++) { // draw planets/moons in orbit
    for(size_t i=0; i<planets.size(); i++) {
        drawPlanet(planets[i], program, info, textures, models, matrix);
    }
    for(size_t i=0; i<explosions.size(); i++) {
        drawExplosion(explosions[i], program, textures, models, matrix);
    }
    glBindVertexArray(0);
}


// ============================================================
// UPDATE FUNCTIONS
// ============================================================

void updateEverything(std::vector<Planet>* planets, std::vector<Planet>* explosions, Info* info) {
    std::set<int> collideSet;
    for(size_t i=0; i<planets->size(); i++) {
        Planet& planet = planets->operator[](i);
        // MOVEMENT
        if(glm::length(planet.position) > float(Planet::distanceMax)) { // hitbox
            planet.direction = -1.0f * planet.direction;
            planet.inclination = -1.0f * planet.inclination;
        }
        else if(planet.dirUpdateNb % Planet::dirUpdateRate == 0) { // chance of changing direction
            planet.direction = glm::normalize(glm::sphericalRand(1.0f));
            planet.dirUpdateNb = 0;
        }
        planet.position += planet.direction;
        planet.dirUpdateNb++;
        // COLLISION DETECTION
        if(!planet.hasLoaded || collideSet.find(i) != collideSet.end()) continue; // skip collision if not loaded
        for(size_t j=0; j<planets->size(); j++) {
            if(i == j || collideSet.find(j) != collideSet.end()) continue;
            Planet& other = planets->operator[](j);
            if(!other.hasLoaded) continue;
            if(planet.hasCollided(other)) { // collision detected
                std::cout << "Collision! (" << i << ", " << j << ")" << std::endl;
                if((planet.size > Planet::minC && other.size > Planet::minC) || (planet.size <= Planet::minC && other.size <= Planet::minC)) {
                    collideSet.insert(i);
                    collideSet.insert(j); }
                else collideSet.insert((planet.size <= Planet::minC ? i : j));
                // planets->operator[]((destroyed == i) ? j : i).size *= 1.5;
            }
        }
    }
    // COLLISION RESULT
    int nbC = 0; int sizeC = 0; glm::vec3 posC;
    for(auto i = collideSet.rbegin(); i != collideSet.rend(); i++) {
        auto planet = planets->begin() + *i; // get collided planet
        if(planet->size > Planet::minC) {
            if(planet->size > sizeC) sizeC = planet->size;
            posC = planet->position; nbC++; }
        addExplosion(explosions, info->getTime(), planet->size, planet->position);
        planets->erase(planet);
        if(nbC == 2) { // create new planets (exploding fragments)
            float s = float(sizeC) / 2.0;
            nbC = 0; sizeC = 0;
            if(s < Planet::minC) continue; // only accept not too small planets
            int NB = Planet::selectExplodingFragments();
            for(int n=0; n<NB; n++) {
                planets->push_back(createPlanet(info->getTime(), s, posC));
            }
        }
    }
    // EXPLOSION EFFECTS
    bool fRem = false;
    for(size_t i=0; i<explosions->size(); i++) { // explosion particles
        explosions->operator[](i).position += explosions->operator[](i).direction * Planet::explosionSpeed;
        explosions->operator[](i).size /= Planet::explosionRate;
        if(!fRem && explosions->operator[](i).size <= Planet::explosionMinSize) fRem = true;
    }
    if(fRem) explosions->erase(std::remove_if(explosions->begin(), explosions->end(),
                [](const Planet& e) {return e.size <= Planet::explosionMinSize;}), explosions->end());
    // SPECIAL EVENTS
    if(info->specialSpawn()) { // spawn a new planet
        std::cout << "Spawning a new planet!" << std::endl;
        planets->push_back(createPlanet(info->getTime()));
        info->modifySpecialSpawn(); }
    if(info->specialClean()) { // delete all small planets
        std::cout << "Deleting all small planets." << std::endl;
        planets->erase(std::remove_if(planets->begin(), planets->end(),
            [](const Planet& p) {return p.size <= Planet::minC;}), planets->end());
        info->modifySpecialClean();
    }
}


void updateVisibility(std::vector<Planet>* planets, Info info) {
    for(size_t i=0; i<planets->size(); i++) {
        Planet& planet = planets->operator[](i);
        if(planet.hasLoaded) continue; // skip loaded planets
        if(planet.durationOfLoad - planet.spawnTime >= planet.loadPeriod) { // load finish
            planet.hasLoaded = true;
            planet.visibility = 1.0;
            continue;}
        else planet.durationOfLoad = info.getTime(); // load not finish
        if(planet.visibility >= 1.0) planet.visibilityOp = -0.1;
        if(planet.visibility <= 0.1) planet.visibilityOp = 0.1;
        planet.visibility += planet.visibilityOp; // flashing effect when not loaded
    }
}