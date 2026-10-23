#pragma once

#include <glm/glm.hpp>

struct GLFWwindow;

/// @brief Free camera (WASD + mouse).
class Camera {
public:
  Camera(glm::vec3 startPos = glm::vec3(5.0f));

  /// @brief Movement handler for keyboard (WASD, Space/C).
  void processInput(GLFWwindow *window, float deltaTime);

  /// @brief Movement handler for mouse (call from the callback).
  void processMouse(double xpos, double ypos);

  glm::mat4 getViewMatrix() const;

  glm::mat4 getProjectionMatrix(float aspectRatio) const;

  void resetMouseState();

private:
  glm::vec3 position_;
  glm::vec3 front_;
  glm::vec3 up_;
  glm::vec3 right_;

  float yaw_ = -135.0f;  ///< Horizon (degree).
  float pitch_ = -30.0f; ///< Vertical (degree).

  float lastX_ = 0.0f;
  float lastY_ = 0.0f;
  bool firstMouse_ = true;

  float speed_ = 10.5f;
  float sensitivity_ = 0.1f;

  void updateVectors();
};
