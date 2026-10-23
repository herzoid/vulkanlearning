#pragma once

#include <GLFW/glfw3.h>
#include <vector>
#include <vulkan/vulkan.h>

class VulkanContext;

/// @brief Manages SwapChain, ImageViews & Framebuffers.
class SwapChain {
public:
  SwapChain(VulkanContext *context, GLFWwindow *window);
  ~SwapChain();

  SwapChain(const SwapChain &) = delete;
  SwapChain &operator=(const SwapChain &) = delete;

  VkSwapchainKHR getSwapChain() const { return swapChain_; }
  VkFormat getImageFormat() const { return swapChainImageFormat_; }
  VkExtent2D getExtent() const { return swapChainExtent_; }
  const std::vector<VkImage> &getImages() const { return swapChainImages_; }
  const std::vector<VkImageView> &getImageViews() const {
    return swapChainImageViews_;
  }
  const std::vector<VkFramebuffer> &getFramebuffers() const {
    return swapChainFramebuffers_;
  }

  // void recreate(VkRenderPass renderPass, VkImageView colorImageView,
  //               VkImageView depthImageView, VkSampleCountFlagBits
  //               msaaSamples);

  void createFramebuffers(VkRenderPass renderPass, VkImageView colorImageView,
                          VkImageView depthImageView,
                          VkSampleCountFlagBits msaaSamples);

private:
  VulkanContext *context_;
  GLFWwindow *window_;
  VkSwapchainKHR swapChain_ = VK_NULL_HANDLE;
  std::vector<VkImage> swapChainImages_;
  VkFormat swapChainImageFormat_;
  VkExtent2D swapChainExtent_;
  std::vector<VkImageView> swapChainImageViews_;
  std::vector<VkFramebuffer> swapChainFramebuffers_;

  void createSwapChain();
  void createImageViews();
  void cleanup();

  // Helpers
  VkSurfaceFormatKHR chooseSwapSurfaceFormat(
      const std::vector<VkSurfaceFormatKHR> &availableFormats);

  VkPresentModeKHR chooseSwapPresentMode(
      const std::vector<VkPresentModeKHR> &availablePresentModes);

  VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities);

  VkImageView createImageView(VkImage image, VkFormat format,
                              VkImageAspectFlags aspectFlags,
                              uint32_t mipLevels);
};
