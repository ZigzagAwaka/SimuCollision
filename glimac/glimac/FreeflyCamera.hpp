#pragma once

#include <iostream>
#include "glm.hpp"


namespace glimac {

class FreeflyCamera {
    private:
        glm::vec3 m_Position;
        float m_fPhi;
        float m_fTheta;
        glm::vec3 m_FrontVector;
        glm::vec3 m_LeftVector;
        glm::vec3 m_UpVector;
        glm::vec3 worldUp = glm::vec3(0.0, 1.0, 0.0);

        void computeDirectionVectors() {
            m_FrontVector = glm::normalize(glm::vec3(glm::cos(m_fTheta)*glm::sin(m_fPhi),
                                                     glm::sin(m_fTheta),
                                                     glm::cos(m_fTheta)*glm::cos(m_fPhi)));
            m_LeftVector = glm::normalize(glm::cross(worldUp, m_FrontVector));
            m_UpVector = glm::normalize(glm::cross(m_FrontVector, m_LeftVector));
        }

    public:
        FreeflyCamera() : m_Position {glm::vec3(0.0, 0.0, 5.0)},
                          m_fPhi {glm::pi<float>()}, m_fTheta {0.0} {
            computeDirectionVectors();
        }

        void moveLeft(float t) {
            m_Position += t*m_LeftVector;
        }

        void moveFront(float t) {
            m_Position -= t*m_FrontVector;
        }

        void rotateLeft(float degrees) {
            computeDirectionVectors();
            m_fPhi += glm::radians(degrees);
            computeDirectionVectors();
        }

        void rotateUp(float degrees) {
            computeDirectionVectors();
            m_fTheta += glm::radians(degrees);
            computeDirectionVectors();
        }

        glm::mat4 getViewMatrix() const {
            return glm::lookAt(m_Position, m_Position + m_FrontVector, m_UpVector);
        }
};

}