#include "camera.hpp"

// #define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <glm/gtc/matrix_transform.hpp>

Camera::Camera(glm::vec3 startPos)
    : position_(startPos), front_(glm::vec3(0.0f, 0.0f, -1.0f)),
      up_(glm::vec3(0.0f, 1.0f, 0.0f)) {
  updateVectors();
}

void Camera::processInput(GLFWwindow *window, float deltaTime) {
  // Движение камеры (WASD)
  float speed = speed_ * deltaTime;

  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
    position_ += front_ * speed;
  }
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
    position_ -= front_ * speed;
  }
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
    position_ -= right_ * speed;
  }
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
    position_ += right_ * speed;
  }

  // Движение вверх/вниз c и space)
  if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
    position_ -= up_ * speed;
  }
  if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
    position_ += up_ * speed;
  }

  // Exit
  // if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
  //   glfwSetWindowShouldClose(window, GLFW_TRUE);
  // }
}


void Camera::processMouse(double xpos, double ypos) {
    if (firstMouse_) {
        lastX_ = xpos;
        lastY_ = ypos;
        firstMouse_ = false;
    }

    float xoffset = (xpos - lastX_) * sensitivity_;
    float yoffset = (lastY_ - ypos) * sensitivity_;
    lastX_ = xpos;
    lastY_ = ypos;

    yaw_ += xoffset;
    pitch_ += yoffset;

    if (pitch_ > 89.0f) pitch_ = 89.0f;
    if (pitch_ < -89.0f) pitch_ = -89.0f;

    updateVectors();
}

void Camera::updateVectors() {
    glm::vec3 front;
    front.x = std::cos(glm::radians(yaw_)) * std::cos(glm::radians(pitch_));
    front.y = std::sin(glm::radians(pitch_));
    front.z = std::sin(glm::radians(yaw_)) * std::cos(glm::radians(pitch_));
    front_ = glm::normalize(front);
    right_ = glm::normalize(glm::cross(front_, up_));
}

glm::mat4 Camera::getViewMatrix() const {
    return glm::lookAt(position_, position_ + front_, up_);
}

glm::mat4 Camera::getProjectionMatrix(float aspectRatio) const {
    glm::mat4 proj = glm::perspective(
        glm::radians(45.0f), aspectRatio, 0.1f, 100.0f);
    proj[1][1] *= -1;  // Vulkan Y-flip
    return proj;
}

void Camera::resetMouseState() {
    firstMouse_ = true;
}
