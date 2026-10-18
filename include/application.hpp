#pragma once

#include "vertex.hpp"
#include <sys/types.h>


#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <cstdint>
#include <optional>
#include <string>
#include <vector>
#include <vulkan/vulkan.h>


/// @brief Главный класс Vulkan-приложения.
///
/// Управляет жизненным циклом: инициализация Vulkan, главный цикл рендеринга,
/// очистка ресурсов. Поддерживает загрузку моделей в формате .glb (бинарный glTF).
class HelloTriangleApplication {
public:
  HelloTriangleApplication(std::string modelPath)
      : modelPath(std::move(modelPath)) {}
  void setWindowSize(uint32_t W, uint32_t H);
  void run();

private:
  struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    bool isComplete() {
      return graphicsFamily.has_value() && presentFamily.has_value();
    }
  };

  struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
  };

  std::string modelPath;
  uint32_t windowWidth = 800;
  uint32_t windowHeight = 600;

  GLFWwindow *window;

  VkInstance instance;
  VkDebugUtilsMessengerEXT debugMessenger;
  VkSurfaceKHR surface;

  VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
  VkDevice device;
  VkSampleCountFlagBits msaaSamples = VK_SAMPLE_COUNT_1_BIT;

  VkImage colorImage;
  VkDeviceMemory colorImageMemory;
  VkImageView colorImageView;

  VkQueue graphicsQueue;
  VkQueue presentQueue;

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

  uint32_t mipLevels;
  VkImage textureImage;
  VkDeviceMemory textureImageMemory;
  VkImageView textureImageView;
  VkSampler textureSampler;

  std::vector<unsigned char> loadedPixels;
  int loadedTexWidth = 0;
  int loadedTexHeight = 0;

  struct TextureData{
    std::vector<u_char> pixels;
    int width = 0;
    int height = 0;
    int imageIndex = -1;
  };

  std::vector<TextureData> loadedTextures;

  std::vector<Vertex> vertices;

  // Вектор, из которого GPU узнаёт в каком порядке соединять вершины в
  // треугольники
  std::vector<uint32_t> indices;

  VkBuffer vertexBuffer;
  VkDeviceMemory vertexBufferMemory;
  VkBuffer indexBuffer;
  VkDeviceMemory indexBufferMemory;

  std::vector<VkBuffer> uniformBuffers;
  std::vector<VkDeviceMemory> uniformBuffersMemory;
  std::vector<void *> uniformBuffersMapped;

  VkDescriptorPool descriptorPool;
  std::vector<VkDescriptorSet> descriptorSets;

  std::vector<VkCommandBuffer> commandBuffers;

  std::vector<VkSemaphore> imageAvailableSemaphores;
  std::vector<VkSemaphore> renderFinishedSemaphores;
  std::vector<VkFence> inFlightFences;
  uint32_t currentFrame = 0;

  bool framebufferResized = false;

  // Камера
  glm::vec3 cameraPos = glm::vec3(5.0f, 5.0f, 5.0f);
  glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
  glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

  float yaw = 0.0f;   // Поворот по горизонтали
  float pitch = 0.0f; // Поворот по вертикали
  float lastX = windowWidth / 2.0f;
  float lastY = windowHeight / 2.0f;
  bool firstMouse = true;

  float cameraSpeed = 10.5f;
  float mouseSensitivity = 0.1f;

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

  void createInstance();

  void populateDebugMessengerCreateInfo(
      VkDebugUtilsMessengerCreateInfoEXT &createInfo);

  void setupDebugMessenger();

  void createSurface();

  void pickPhysicalDevice();

  void createLogicalDevice();

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

  void loadModel();

  bool loadGltfModel(const std::string &filename, std::vector<Vertex> &vertices,
                     std::vector<uint32_t> &indices);

  void createVertexBuffer();

  VkSampleCountFlagBits getMaxUsableSampleCount();

  void createTextureImage();

  void generateMipmaps(VkImage image, VkFormat imageFormat, int32_t texWidth,
                       int32_t texHeight, uint32_t mipLevels);

  void createTextureImageView();

  void createTextureSampler();

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

  SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);

  bool isDeviceSuitable(VkPhysicalDevice device);

  bool checkDeviceExtensionSupport(VkPhysicalDevice device);

  QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);

  std::vector<const char *> getRequiredExtensions();

  bool checkValidationLayerSupport();

  static std::vector<char> readFile(const std::string &filename);

  static VKAPI_ATTR VkBool32 VKAPI_CALL
  debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                VkDebugUtilsMessageTypeFlagsEXT messageType,
                const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
                void *pUserData);

  void processInput(float deltaTime);

  static void mouseCallback(GLFWwindow *window, double xpos, double ypos);
};
