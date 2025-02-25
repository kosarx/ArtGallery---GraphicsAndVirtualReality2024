#pragma once
#include <glm/glm.hpp>

class Light {
public:

    GLFWwindow* window;
    // Light parameters
    glm::mat4 viewMatrix;
    glm::mat4 projectionMatrix;

    glm::vec3 lightPosition_worldspace;

    glm::vec4 La;
    glm::vec4 Ld;
    glm::vec4 Ls;

    float kc;
    float kl;
    float kq;
    float power;

    float nearPlane;
    float farPlane;

    bool orthoProj;

    float lightSpeed;
    glm::vec3 direction;

    // Where the light will look at
    glm::vec3 targetPosition;

    // Constructor
    Light(GLFWwindow* window,
        glm::vec4 init_La = glm::vec4(0,0,0,0),
        glm::vec4 init_Ld = glm::vec4(0, 0, 0, 0),
        glm::vec4 init_Ls = glm::vec4(0, 0, 0, 0),
        float init_kc = 1.0f,
        float init_kl = 0.09f,
        float init_kq = 0.032f, //some values
        glm::vec3 init_position = glm::vec3(0, 0, 0),
        float init_power = 0
    );
  
    void update();

    glm::mat4 lightVP();

    void updateProjectionMatrix();
};
