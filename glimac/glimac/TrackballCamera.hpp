#pragma once

#include "glm.hpp"


namespace glimac {

class TrackballCamera {
    private:
        float m_fDistance;
        float m_fAngleX;
        float m_fAngleY;

    public:
        TrackballCamera() : m_fDistance {5.0}, m_fAngleX {15.0}, m_fAngleY {45.0} {}

        void moveFront(float delta) {
            m_fDistance -= delta;
        }

        void rotateLeft(float degrees) {
            m_fAngleY += degrees;
        }

        void rotateUp(float degrees) {
            m_fAngleX += degrees;
        }

        glm::mat4 getViewMatrix() const {
            glm::mat4 viewMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, -1.0 * m_fDistance));
            viewMatrix = glm::rotate(viewMatrix, glm::radians(m_fAngleX), glm::vec3(1, 0, 0));
            viewMatrix = glm::rotate(viewMatrix, glm::radians(m_fAngleY), glm::vec3(0, 1, 0));
            return viewMatrix;
        }
};

}