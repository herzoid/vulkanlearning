#include "model.hpp"

#include <fstream>
#include <iostream>
#include <stdexcept>

#define STB_IMAGE_IMPLEMENTATION
#include <external/stb_image.h>

#define TINYGLTF_IMPLEMENTATION
#define TINYGLTF_NO_STB_IMAGE
#define TINYGLTF_NO_STB_IMAGE_WRITE
#include <tinygltf/tiny_gltf.h>

LoadedModel loadGltfModel(const std::string &filename) {
  tinygltf::Model model;
  tinygltf::TinyGLTF loader;
  std::string err, warn;

  // ---------Callback for images decoding------------
  loader.SetImageLoader(
      [](tinygltf::Image *image, const int image_idx, std::string *err,
         std::string *warn, int req_width, int req_height,
         const unsigned char *bytes, int size, void *user_data) -> bool {
        int w, h, ch;
        stbi_uc *pixels =
            stbi_load_from_memory(bytes, size, &w, &h, &ch, STBI_rgb_alpha);

        if (!pixels) {
          if (err)
            *err += " stbi_load_from_memory failed";
          return false;
        }

        image->width = w;
        image->height = h;
        image->component = 4;
        image->bits = 8;
        image->pixel_type = TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE;
        image->image.assign(pixels, pixels + (w * h * 4));

        stbi_image_free(pixels);
        return true;
      },
      nullptr // user_data
  );

  // ----------Checking for magic header--------------
  std::ifstream file(filename, std::ios::binary);
  if (!file.is_open()) {
    throw std::runtime_error("Failed to open file: " + filename);
  }

  char magic[4] = {0};
  file.read(magic, 4);
  file.close();

  if (magic[0] != 'g' || magic[1] != 'l' || magic[2] != 'T' ||
      magic[3] != 'F') {
    throw std::runtime_error(
        "Model format not supported. Only binary glTF (.glb) is allowed.\n");
  }

  // ----------Loading model---------------
  bool ret = false;
  ret = loader.LoadBinaryFromFile(&model, &err, &warn, filename);

  if (!warn.empty()) {
    std::cout << "TinyGLTF warning: " << warn << std::endl;
  }

  if (!ret) {
    std::cerr << "Failed to load glTF file: " << filename << ". Error: " << err
              << std::endl;
  }

  std::cout << "Meshes: " << model.meshes.size()
            << "\nImages: " << model.images.size()
            << "\nMaterials: " << model.materials.size()
            << "\nTextures: " << model.textures.size() << std::endl;

  LoadedModel result;
  
  //-----------Loading all textures (only pixels)-----------
  for (const auto &img : model.images) {
    TextureData tex;
    tex.width = img.width;
    tex.height = img.height;
    tex.pixels = img.image;
    result.textures.push_back(std::move(tex));
  }

  //-----------Fallback texture--------------
  if (result.textures.empty()) {
    std::cout << "No textures in model, adding white fallback color"
              << std::endl;
    TextureData fallback;
    fallback.width = 1;
    fallback.height = 1;
    fallback.pixels = {255, 255, 255, 255};
    result.textures.push_back(std::move(fallback));
  }

  //--------Find all meshes and primitives-----------
  for (const auto &mesh : model.meshes) {
    for (const auto &primitive : mesh.primitives) {
      if (primitive.mode != TINYGLTF_MODE_TRIANGLES)
        continue;
      // Define the texture for the primitive
      int32_t texIdx = -1;
      if (primitive.material >= 0) {
        const auto &mat = model.materials[primitive.material];
        int texInfoIdx = mat.pbrMetallicRoughness.baseColorTexture.index;
        if (texInfoIdx >= 0 && texInfoIdx < (int)model.images.size()) {
          int imgIdx = model.textures[texInfoIdx].source;
          if (imgIdx >= 0 && imgIdx < (int)model.images.size()) {
            texIdx = imgIdx;
          }
        }
      }
      // If no texture - use fallback texture
      if (texIdx < 0) {
        texIdx = static_cast<int>(result.textures.size()) - 1;
      }

      // Vertices positions
      auto posIt = primitive.attributes.find("POSITION");
      if (posIt == primitive.attributes.end())
        continue;

      const auto &posAcc = model.accessors[posIt->second];
      const auto &posView = model.bufferViews[posAcc.bufferView];
      const auto &posBuf = model.buffers[posView.buffer];
      const float *positions = reinterpret_cast<const float *>(
          &posBuf.data[posView.byteOffset + posAcc.byteOffset]);

      //------Texture coordinates UV----------
      const float *texCoords = nullptr;
      auto texIt = primitive.attributes.find("TEXCOORD_0");
      if (texIt != primitive.attributes.end()) {
        const auto &texAcc = model.accessors[texIt->second];
        const auto &texView = model.bufferViews[texAcc.bufferView];
        const auto &texBuf = model.buffers[texView.buffer];
        texCoords = reinterpret_cast<const float *>(
            &texBuf.data[texView.byteOffset + texAcc.byteOffset]);
      }

      // The begining of the primitive's vertices
      uint32_t vertexBase = static_cast<uint32_t>(result.vertices.size());

      //---------Filling the vertices-----------------
      for (size_t i = 0; i < posAcc.count; ++i) {
        Vertex v{};
        v.pos = {positions[i * 3 + 0], positions[i * 3 + 1],
                 positions[i * 3 + 2]};
        v.texCoord = texCoords
                         ? glm::vec2(texCoords[i * 2 + 0], texCoords[i * 2 + 1])
                         : glm::vec2(0.0f);
        v.color = glm::vec3(1.0f);
        result.vertices.push_back(v);
      }

      // The beginning of the indices
      uint32_t indexStart = static_cast<uint32_t>(result.indices.size());

      //---------Filling the indices-------------
      if (primitive.indices >= 0) {
        const auto &indAcc = model.accessors[primitive.indices];
        const auto &indView = model.bufferViews[indAcc.bufferView];
        const auto &indBuf = model.buffers[indView.buffer];
        const void *indData =
            &indBuf.data[indView.byteOffset + indAcc.byteOffset];
        for (size_t i = 0; i < indAcc.count; ++i) {
          uint32_t idx = 0;
          if (indAcc.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT) {
            idx = static_cast<const uint32_t *>(indData)[i];
          } else if (indAcc.componentType ==
                     TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT) {
            idx = static_cast<const uint16_t *>(indData)[i];
          } else if (indAcc.componentType ==
                     TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE) {
            idx = static_cast<const uint8_t *>(indData)[i];
          }
          result.indices.push_back(vertexBase + idx); // Offset
        }
      } else {
        for (size_t i = 0; i < posAcc.count; ++i) {
          result.indices.push_back(vertexBase + static_cast<uint32_t>(i));
        }
      }

      uint32_t indexCount = static_cast<uint32_t>(result.indices.size()) - indexStart;

      //-------Saving mesh----------
      result.meshes.push_back(Mesh{indexStart, indexCount, texIdx});

      std::cout << "Mesh: " << indexCount << " indices, texture " << texIdx
                << std::endl;
    }
  }

  std::cout << "Total: " << result.vertices.size() << " vertices, " << result.indices.size()
            << " indices, " << result.meshes.size() << " meshes, "
            << result.textures.size() << " textures" << std::endl;
  return result;
}
