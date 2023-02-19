#include "camera.h"

Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch)
    : position(position),
      up(up),
      yaw(yaw),
      pitch(pitch),
      front(glm::vec3(0.0f, 0.0f, -1.0f)),
      movementSpeed(SPEED),
      mouseSensitivity(SENSITIVITY),
      zoom(ZOOM) {
    updateCameraVectors();
}

glm::mat4 Camera::getViewMatrix() {
    return glm::lookAt(position, position + front, up);
}

void Camera::processKeyboard(Camera_Movement direction, float deltaTime) {
    float velocity = movementSpeed * deltaTime;
    switch (direction) {
        case FORWARD:
            position += front * velocity;
            break;
        case BACKWARD:
            position -= front * velocity;
            break;
        case LEFT:
            position -= right * velocity;
            break;
        case RIGHT:
            position += right * velocity;
            break;
    }
}

void Camera::processMouseMovement(float xoffset, float yoffset,
                                  GLboolean constrainPitch) {
    xoffset *= mouseSensitivity;
    yoffset *= mouseSensitivity;

    this->yaw += xoffset;
    this->pitch += yoffset;

    if (constrainPitch) {
        if (this->pitch > 89.0f) this->pitch = 89.0f;
        if (this->pitch < -89.0f) this->pitch = -89.0f;
    }
    updateCameraVectors();
}

void Camera::processMouseScroll(float yoffset) {
    this->zoom -= static_cast<float>(yoffset);
    if (this->zoom < 1.0f) this->zoom = 1.0f;
    if (this->zoom > 45.0f) this->zoom = 45.0f;
}

void Camera::updateCameraVectors() {
    glm::vec3 front{};
    front.x =
        glm::cos(glm::radians(this->yaw)) * glm::cos(glm::radians(this->pitch));
    front.y = glm::sin(glm::radians(this->pitch));
    front.z =
        glm::sin(glm::radians(this->yaw)) * glm::cos(glm::radians(this->pitch));
    this->front = glm::normalize(front);

    this->right = glm::normalize(glm::cross(this->front, this->up));
    this->up = glm::normalize(glm::cross(this->right, this->front));
}