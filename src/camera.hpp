#pragma once

#include <glimac/TrackballCamera.hpp>
#include <glimac/FreeflyCamera.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/geometric.hpp>


/* Structure to manage the camera */
struct Camera {
    int CAMERA_TYPE; // 0=trackball, 1=freefly
    float MOVE_SPEED = 120.0; // zoom, moveFront/Left speed of the camera
    float ROTATION_SPEED = 2.0; // rotation, rotateLeft/Up speed of the camera
    glimac::TrackballCamera trackballCamera;
    glimac::FreeflyCamera freeflyCamera;

    Camera(int type) : CAMERA_TYPE {type} {}

    void moveFront(float offset) {
        if(CAMERA_TYPE == 0) trackballCamera.moveFront(MOVE_SPEED * offset);
        else freeflyCamera.moveFront(MOVE_SPEED * offset);
    }

    void moveLeft(float offset) {
        if(CAMERA_TYPE == 0) return;
        else freeflyCamera.moveLeft(MOVE_SPEED * offset);
    }

    void rotateLeft(float degrees) {
        if(CAMERA_TYPE == 0) trackballCamera.rotateLeft(degrees * ROTATION_SPEED);
        else freeflyCamera.rotateLeft(degrees * ROTATION_SPEED);
    }

    void rotateUp(float degrees) {
        if(CAMERA_TYPE == 0) trackballCamera.rotateUp(degrees * ROTATION_SPEED);
        else freeflyCamera.rotateUp(degrees * ROTATION_SPEED);
    }

    void switchType() {
        if(CAMERA_TYPE == 0) CAMERA_TYPE = 1;
        else CAMERA_TYPE = 0;
    }

    glm::mat4 getViewMatrix() {
        if(CAMERA_TYPE == 0) return trackballCamera.getViewMatrix();
        else return freeflyCamera.getViewMatrix();
    }

    /* Get the ModelViewMatrix based on ModelMatrix given in parameter */
    glm::mat4 getGlobalMVMatrix(glm::mat4 modelMatrix) {
        if(CAMERA_TYPE == 0) return modelMatrix * getViewMatrix();
        else return getViewMatrix() * modelMatrix;
    }
};
