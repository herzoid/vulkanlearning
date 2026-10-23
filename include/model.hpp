#pragma once

#include "vertex.hpp"
#include <string>

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

/// @brief Load model from file .glb (binary glTF).
///
/// Multi meshes and textures support. Load geometry,
/// UV and textures (as a pixels, without GPU-objects).
///
/// @param filename path to .glb.
/// @return LoadedModel with vertices, indices, meshs and textures.
/// @throws std::runtime_error If any errors opening .glb.
LoadedModel loadGltfModel(const std::string &filename);
