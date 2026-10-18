#pragma once

#include <array>
#include <cstddef>
#include <glm/glm.hpp>
#include <vulkan/vulkan.h>

struct Vertex {
  glm::vec3 pos;
  glm::vec3 color;
  glm::vec2 texCoord;

  static VkVertexInputBindingDescription getBindingDescription();

  static std::array<VkVertexInputAttributeDescription, 3>
  getAttributeDescriptions();

  bool operator==(const Vertex &other) const;
};

namespace std {
template <> struct hash<Vertex> {
  size_t operator()(Vertex const &vertex) const;
};
} // namespace std
