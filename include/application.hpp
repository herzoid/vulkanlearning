#pragma once

// #define GLFW_INCLUDE_VULKAN

#include "camera.hpp"
#include "model.hpp"
#include "vulkan_context.hpp"
#include <memory>
#include <sys/types.h>
#include <vulkan/vulkan_core.h>

#include <GLFW/glfw3.h>

// #define GLM_FORCE_RADIANS
// #define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <cstdint>
#include <string>
#include <vector>
#include <vulkan/vulkan.h>

/// @brief Главный класс Vulkan-приложения.
///
/// Управляет жизненным циклом: инициализация Vulkan, главный цикл рендеринга,
/// очистка ресурсов. Поддерживает загрузку моделей в формате .glb (бинарный
/// glTF).
class HelloTriangleApplication {
public:
  HelloTriangleApplication(std::string modelPath)
      : modelPath(std::move(modelPath)) {}
  void setWindowSize(uint32_t W, uint32_t H);
  void run();

private:
  /// @brief Loaded texture info.
  struct Texture {
    VkImage image = VK_NULL_HANDLE;
    VkDeviceMemory memory = VK_NULL_HANDLE;
    VkImageView view = VK_NULL_HANDLE;
    VkSampler sampler = VK_NULL_HANDLE;
    uint32_t mipLevels = 1;
    int width = 0;
    int height = 0;
  };

  std::unique_ptr<VulkanContext> context_;

  Camera camera;

  std::string modelPath;
  uint32_t windowWidth = 800;
  uint32_t windowHeight = 600;

  GLFWwindow *window;

  VkSampleCountFlagBits msaaSamples = VK_SAMPLE_COUNT_1_BIT;

  VkImage colorImage;
  VkDeviceMemory colorImageMemory;
  VkImageView colorImageView;

  VkSwapchainKHR swapChain;
  std::vector<VkImage> swapChainImages;
  VkFormat swapChainImageFormat;
  VkExtent2D swapChainExtent;
  std::vector<VkImageView> swapChainImageViews;
  std::vector<VkFramebuffer> swapChainFramebuffers;

  VkRenderPass renderPass;
  VkDescriptorSetLayout descriptorSetLayout;
  VkPipelineLayout pipelineLayout;
  VkPipeline graphicsPipeline;

  VkCommandPool commandPool;

  VkImage depthImage;
  VkDeviceMemory depthImageMemory;
  VkImageView depthImageView;

  std::vector<Texture> textures; ///< GPU textures.

  LoadedModel model;

  VkBuffer vertexBuffer;
  VkDeviceMemory vertexBufferMemory;
  VkBuffer indexBuffer;
  VkDeviceMemory indexBufferMemory;

  std::vector<VkBuffer> uniformBuffers;
  std::vector<VkDeviceMemory> uniformBuffersMemory;
  std::vector<void *> uniformBuffersMapped;

  VkDescriptorPool descriptorPool;
  std::vector<std::vector<VkDescriptorSet>> descriptorSets;

  std::vector<VkCommandBuffer> commandBuffers;

  std::vector<VkSemaphore> imageAvailableSemaphores;
  std::vector<VkSemaphore> renderFinishedSemaphores;
  std::vector<VkFence> inFlightFences;
  uint32_t currentFrame = 0;

  bool framebufferResized = false;

  bool isFullscreen = false;
  int windowedX = 0, windowedY = 0;
  int windowedWidth = windowWidth, windowedHeight = windowHeight;

  void initWindow();
  void toggleFullscreen();

  static void framebufferResizeCallback(GLFWwindow *window, int width,
                                        int height);

  void initVulkan();

  void mainLoop();
  void cleanupSwapChain();

  void cleanup();

  void recreateSwapChain();

  void createSwapChain();

  void createImageViews();

  void createRenderPass();

  void createDescriptorSetLayout();

  void createGraphicsPipeline();

  void createFramebuffers();

  void createCommandPool();

  void createColorResources();

  void createDepthResources();

  VkFormat findSupportedFormat(const std::vector<VkFormat> &candidates,
                               VkImageTiling tiling,
                               VkFormatFeatureFlags features);

  VkFormat findDepthFormat();

  bool hasStencilComponent(VkFormat format);

  void createVertexBuffer();

  void createTextures();

  void generateMipmaps(VkImage image, VkFormat imageFormat, int32_t texWidth,
                       int32_t texHeight, uint32_t mipLevels);

  // void createTextureImageView();

  // void createTextureSampler();

  VkImageView createImageView(VkImage image, VkFormat format,
                              VkImageAspectFlags aspectFlags,
                              uint32_t mipLevels);

  void createImage(uint32_t width, uint32_t height, uint32_t mipLevels,
                   VkSampleCountFlagBits numSamples, VkFormat format,
                   VkImageTiling tiling, VkImageUsageFlags usage,
                   VkMemoryPropertyFlags properties, VkImage &image,
                   VkDeviceMemory &imageMemory);

  void transitionImageLayout(VkImage image, VkFormat format,
                             VkImageLayout oldLayout, VkImageLayout newLayout,
                             uint32_t mipLevels);

  void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width,
                         uint32_t height);

  void createIndexBuffer();
  void createUniformBuffers();
  void createDescriptorPool();

  void createDescriptorSets();
  void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage,
                    VkMemoryPropertyFlags properties, VkBuffer &buffer,
                    VkDeviceMemory &bufferMemory);

  VkCommandBuffer beginSingleTimeCommands();

  void endSingleTimeCommands(VkCommandBuffer commandBuffer);

  void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

  uint32_t findMemoryType(uint32_t typeFilter,
                          VkMemoryPropertyFlags properties);

  void createCommandBuffers();

  void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);

  void createSyncObjects();

  void updateUniformBuffer(uint32_t currentImage);

  void drawFrame();
  VkShaderModule createShaderModule(const std::vector<char> &code);

  VkSurfaceFormatKHR chooseSwapSurfaceFormat(
      const std::vector<VkSurfaceFormatKHR> &availableFormats);

  VkPresentModeKHR chooseSwapPresentMode(
      const std::vector<VkPresentModeKHR> &availablePresentModes);

  VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities);

  static std::vector<char> readFile(const std::string &filename);

  static void mouseCallback(GLFWwindow *window, double xpos, double ypos);
};
