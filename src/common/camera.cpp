#include <glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include "camera.h"
#include <iostream>
using namespace glm;

Camera::Camera(GLFWwindow* window) : window(window) {
    position = vec3(0, 16, 0);
    horizontalAngle = 3.14f;
    verticalAngle = 0.0f;
    FoV = 65.0f;
    speed = 1/2.0f * 25.0f;
    mouseSpeed = 0.001f;
    fovSpeed = 2.0f;
    direction = vec3(0, 0, -1);
    right = vec3(-1, 0, 0);
    up = vec3(0, 1, 0);
    deltaTime = 0;
}

void Camera::pollMovement() {
    // Task 5.1: simple camera movement that moves in +-z and +-x axes
    // Move forward
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        position += direction * deltaTime * speed * vec3(1, 0, 1);
        //std::cout << position.x << << ", " << position.y << position.z << std::endl;
    }
    // Move backward
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        position -= direction * deltaTime * speed * vec3(1, 0, 1);
    }
    // Strafe right
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        position += right * deltaTime * speed * vec3(1, 0, 1);
    }
    // Strafe left
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        position -= right * deltaTime * speed * vec3(1, 0, 1);
    }
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
        position += vec3(0, 1, 0) * deltaTime * speed;
    }
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
        position -= vec3(0, 1, 0) * deltaTime * speed;
    }

    // Task 5.6: handle zoom in/out effects
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        if (radians(FoV) > 0.1 + radians(fovSpeed))
            FoV -= fovSpeed;

    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        if (radians(FoV) < 3.14 - radians(fovSpeed))
            FoV += fovSpeed;
    }

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        FoV = 65.0f;
    }
}

void Camera::update() {
    // glfwGetTime is called only once, the first time this function is called
    static double lastTime = glfwGetTime();

    // Compute time difference between current and last frame
    double currentTime = glfwGetTime();
    deltaTime = float(currentTime - lastTime);

    // Get mouse position
    double xPos, yPos;
    glfwGetCursorPos(window, &xPos, &yPos);

    int width, height;
    glfwGetWindowSize(window, &width, &height);

    // Reset mouse position for next frame
    glfwSetCursorPos(window, width / 2, height / 2);

    // Task 5.3: Compute new horizontal and vertical angles, given windows size
    // and cursor position
    horizontalAngle += mouseSpeed * float(width / 2 - xPos);
    verticalAngle += mouseSpeed * float(height / 2 - yPos);

    // Task 5.4: right and up vectors of the camera coordinate system
    // use spherical coordinates
    direction = vec3(
        cos(verticalAngle) * sin(horizontalAngle),
        sin(verticalAngle),
        cos(verticalAngle) * cos(horizontalAngle)
    );

    // Right vector
    right = vec3(
        sin(horizontalAngle - 3.14f / 2.0f),
        0,
        cos(horizontalAngle - 3.14f / 2.0f)
    );

    // Up vector
    up = cross(right, direction);

    // Task 5.7: construct projection and view matrices
    //projectionMatrix = perspective(radians(FoV), 16.0f / 9.0f, 0.01f, 1000.0f);
    viewMatrix = lookAt(
        position,
        position + direction,
        up
    );

    // For the next frame, the "last time" will be "now"
    lastTime = currentTime;
}

void Camera::setDirection(vec3 directionVector) {
    // Calculate the direction vector from the camera position to the target point
    vec3 targetDirection = normalize(directionVector - position);

    //// Calculate the horizontal angle
    horizontalAngle = atan2(targetDirection.z, targetDirection.x);

    // Calculate the vertical angle
    verticalAngle = asin(targetDirection.y);
    //direction = directionVector;
}

void Camera::setProjectionMatrix(float aspect, float near, float far) {
    projectionMatrix = perspective(radians(FoV), aspect, near, far);
}