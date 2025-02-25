#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <glm/glm.hpp>

class Camera {
public:
    GLFWwindow* window;
    glm::mat4 viewMatrix;
    glm::mat4 projectionMatrix;
    // Initial position : on +Z
    glm::vec3 position;
    // Initial horizontal angle : toward -Z
    float horizontalAngle;
    // Initial vertical angle : none
    float verticalAngle;
    // Field of View
    float FoV;
    float speed; // units / second
    float mouseSpeed;
    float fovSpeed;
    glm::vec3 direction;
    glm::vec3 right;
    glm::vec3 up;
    //float currentTime;
    //float lastTime;
    float deltaTime;

    Camera(GLFWwindow* window);
    /*void updateTime();
    void updateParameters();
    void updateDirVector(float g_verticalAngle, float g_horizontalAngle);
    void updateRightVector(float g_verticalAngle, float g_horizontalAngle);
    void updateUpVector(glm::vec3 g_right, glm::vec3 g_direction);
    void updateProjMatrix();
    void updateViewMatrix();*/
    void update();
    void pollMovement();
    void setDirection(glm::vec3 directionVector);
    void setProjectionMatrix(float aspect, float near, float far);
};

#endif
