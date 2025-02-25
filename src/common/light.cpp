#include <glfw3.h>
#include <iostream>
#include <math.h>
#include <glm/gtc/matrix_transform.hpp>
#include "light.h"

using namespace glm;

Light::Light(GLFWwindow* window,
    glm::vec4 init_La,
    glm::vec4 init_Ld,
    glm::vec4 init_Ls,
    float init_kc,
    float init_kl,
    float init_kq,
    glm::vec3 init_position,
    float init_power) : window(window), kc(init_kc),
    kl(init_kl), kq(init_kq) {
    La = init_La;
    Ld = init_Ld;
    Ls = init_Ls;
    power = init_power;
    lightPosition_worldspace = init_position;

    nearPlane = 1.0;
    farPlane = 100.0;

    direction = normalize(targetPosition - lightPosition_worldspace);

    lightSpeed = 0.1f;
    targetPosition = glm::vec3(0.0, 0.0, 0.0);

    //projectionMatrix = ortho(-10.0f, 10.0f, -10.0f, 10.0f, nearPlane, farPlane);
    projectionMatrix = glm::perspective(glm::radians(45.0f), 1.0f, nearPlane, farPlane);
    orthoProj = false;
}



void Light::update() {


   // Move across z-axis
    if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) {
        lightPosition_worldspace += lightSpeed * vec3(0.0, 0.0, 1.0);
    }
    if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS) {
        lightPosition_worldspace -= lightSpeed * vec3(0.0, 0.0, 1.0);
    }
    // Move across x-axis
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {
        lightPosition_worldspace += lightSpeed * vec3(1.0, 0.0, 0.0);
    }
    if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) {
        lightPosition_worldspace -= lightSpeed * vec3(1.0, 0.0, 0.0);
    }
    // Move across y-axis
    if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS) {
        lightPosition_worldspace += lightSpeed * vec3(0.0, 1.0, 0.0);
    }
    if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) {
        lightPosition_worldspace -= lightSpeed * vec3(0.0, 1.0, 0.0);
    }
    

    //std::cout << lightPosition_worldspace.x << lightPosition_worldspace.y << lightPosition_worldspace.z << std::endl;
    // We have the direction of the light and the point where the light is looking at
    // We will use this information to calculate the "up" vector, 
    // just like we did with the camera

    direction = normalize(targetPosition - lightPosition_worldspace);
    //  projectionMatrix = glm::perspective(glm::radians(45.0f), 1.0f, nearPlane, farPlane);
    
    // converting direction to cylidrical coordinates
    float x = direction.x;
    float y = direction.y;
    float z = direction.z;

    // We don't need to calculate the vertical angle
    
    float horizontalAngle;
    if (z > 0.0) horizontalAngle = atan(x/z);
    else if (z < 0.0) horizontalAngle = atan(x/z) + 3.1415f;
    else horizontalAngle = 3.1415f / 2.0f;

    // Right vector
    vec3 right(
        sin(horizontalAngle - 3.14f / 2.0f),
        0,
        cos(horizontalAngle - 3.14f / 2.0f)
    );
    
    // Up vector
    vec3 up = cross(right, direction);
   
    updateProjectionMatrix();
    viewMatrix = lookAt(
        lightPosition_worldspace,
        targetPosition,
        up//vec3(0.0, 1.0, 0.0)
    );
    //*/

}


mat4 Light::lightVP() {
    return projectionMatrix * viewMatrix;
}

void Light::updateProjectionMatrix() {
    if (orthoProj) {
        // Adjust the orthographic projection matrix based on your needs
        projectionMatrix = glm::ortho(-25.0f, 25.0f, -25.0f, 25.0f, nearPlane, farPlane);
    }
    else {
        // Adjust the perspective projection matrix based on your needs
        projectionMatrix = glm::perspective(glm::radians(45.0f), 1.0f, nearPlane, farPlane);
    }
}