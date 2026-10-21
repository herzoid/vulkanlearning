#pragma once

#include <string>
#include "vertex.hpp"

/// @brief Pixel data, loaded from glTF, before GPU object creation
struct TextureData {
  std::vector<unsigned char> pixels;
  int width = 0;
  int height = 0;
};

/// @brief Mesh for render
struct Mesh {
  uint32_t indexOffset = 0;
  uint32_t indexCount = 0;
  int32_t textureIndex = -1;
};

struct LoadedModel {
  std::vector<Vertex> vertices;
  std::vector<uint32_t> indices;
  std::vector<Mesh> meshes;
  std::vector<TextureData> textures;
};

/// @brief Загружает модель из .glb (бинарный glTF).
///
/// Поддерживает несколько мешей и текстур. Загружает геометрию,
/// UV-координаты и все текстуры (как пиксели, без GPU-объектов).
///
/// @param filename Путь к .glb.
/// @return LoadedModel с вершинами, индексами, мешами и текстурами.
/// @throws std::runtime_error Если файл не .glb или не открывается.
LoadedModel loadGltfModel(const std::string &filename);


