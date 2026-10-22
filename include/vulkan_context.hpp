#pragma once

#include <GLFW/glfw3.h>
#include <optional>
#include <string>
#include <vector>
#include <vulkan/vulkan.h>

/// @brief Хранит базовые Vulkan-объекты: Instance, Device, Queues, Surface.
///
/// Отвечает за инициализацию драйвера, выбор физической видеокарты,
/// создание логического устройства и настройку очередей.
class VulkanContext {
public:
  VulkanContext(GLFWwindow *window);
  ~VulkanContext();

  // Запрещаем копирование, чтобы не было двойного удаления
  VulkanContext(const VulkanContext &) = delete;
  VulkanContext &operator=(const VulkanContext &) = delete;

  // Геттеры для доступа к объектам
  VkInstance getInstance() const { return instance_; }
  VkPhysicalDevice getPhysicalDevice() const { return physicalDevice_; }
  VkDevice getDevice() const { return device_; }
  VkSurfaceKHR getSurface() const { return surface_; }
  VkQueue getGraphicsQueue() const { return graphicsQueue_; }
  VkQueue getPresentQueue() const { return presentQueue_; }

  // Информация о семействах очередей (нужна для SwapChain)
  struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;
    bool isComplete() const {
      return graphicsFamily.has_value() && presentFamily.has_value();
    }
  };

  struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
  };

  QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device) const;
  VkSampleCountFlagBits getMaxUsableSampleCount() const;
  SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device) const;

private:
  GLFWwindow *window_;
  VkInstance instance_;
  VkDebugUtilsMessengerEXT debugMessenger_;
  VkSurfaceKHR surface_;
  VkPhysicalDevice physicalDevice_;
  VkDevice device_;
  VkQueue graphicsQueue_;
  VkQueue presentQueue_;

  // Приватные методы инициализации
  void createInstance();
  void setupDebugMessenger();
  void createSurface();
  void pickPhysicalDevice();
  void createLogicalDevice();

  // Вспомогательные методы
  bool checkValidationLayerSupport() const;
  std::vector<const char *> getRequiredExtensions() const;
  bool isDeviceSuitable(VkPhysicalDevice device) const;
  bool checkDeviceExtensionSupport(VkPhysicalDevice device) const;
  void populateDebugMessengerCreateInfo(
      VkDebugUtilsMessengerCreateInfoEXT &createInfo) const;

  static VKAPI_ATTR VkBool32 VKAPI_CALL
  debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                VkDebugUtilsMessageTypeFlagsEXT messageType,
                const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
                void *pUserData);
};
