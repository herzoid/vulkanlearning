#pragma once

#include <glm/glm.hpp>

struct GLFWwindow;

/// @brief Орбитальная/свободная камера (WASD + мышь).
class Camera {
public:
  Camera(glm::vec3 startPos = glm::vec3(5.0f));

  /// @brief Обрабатывает движение (WASD, Space/C).
  void processInput(GLFWwindow *window, float deltaTime);

  /// @brief Обрабатывает движение мыши (вызывается из callback).
  void processMouse(double xpos, double ypos);

  /// @brief Вычисляет view-матрицу.
  glm::mat4 getViewMatrix() const;

  /// @brief Вычисляет proj-матрицу для заданного aspect ratio.
  glm::mat4 getProjectionMatrix(float aspectRatio) const;

  /// @brief Сбрасывает состояние мыши (для callback).
  void resetMouseState();

private:
  glm::vec3 position_;
  glm::vec3 front_;
  glm::vec3 up_;
  glm::vec3 right_;

  float yaw_ = -135.0f;  ///< Поворот по горизонтали (градусы).
  float pitch_ = -30.0f; ///< Поворот по вертикали (градусы).

  float lastX_ = 0.0f;
  float lastY_ = 0.0f;
  bool firstMouse_ = true;

  float speed_ = 10.5f;
  float sensitivity_ = 0.1f;

  void updateVectors();
};
