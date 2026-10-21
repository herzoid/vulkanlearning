#include "vertex.hpp"
// #define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

VkVertexInputBindingDescription Vertex::getBindingDescription() {
  VkVertexInputBindingDescription bindingDescription{};
  bindingDescription.binding = 0;
  bindingDescription.stride = sizeof(Vertex);
  bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

  return bindingDescription;
}

std::array<VkVertexInputAttributeDescription, 3>
Vertex::getAttributeDescriptions() {
  std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions{};

  // Position
  attributeDescriptions[0].binding = 0;
  attributeDescriptions[0].location = 0;
  attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
  attributeDescriptions[0].offset = offsetof(Vertex, pos);

  // Color
  attributeDescriptions[1].binding = 0;
  attributeDescriptions[1].location = 1;
  attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
  attributeDescriptions[1].offset = offsetof(Vertex, color);

  // Texture coordinates
  attributeDescriptions[2].binding = 0;
  attributeDescriptions[2].location = 2;
  attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
  attributeDescriptions[2].offset = offsetof(Vertex, texCoord);

  return attributeDescriptions;
}

bool Vertex::operator==(const Vertex &other) const {
  return pos == other.pos && color == other.color && texCoord == other.texCoord;
}

namespace std {
size_t hash<Vertex>::operator()(Vertex const &vertex) const {
  return ((hash<glm::vec3>()(vertex.pos) ^
           (hash<glm::vec3>()(vertex.color) << 1)) >>
          1) ^
         (hash<glm::vec2>()(vertex.texCoord) << 1);
}

} // namespace std
