#pragma once

#include "swapchain.hpp"
#include "vulkan_context.hpp"
#include <vector>
#include <vulkan/vulkan.h>

class Camera;
struct LoadedModel;

class Renderer {
public:
  Renderer(VulkanContext *context, SwapChain *swapChain);
  ~Renderer();

  Renderer(const Renderer &) = delete;
  Renderer &operator=(const Renderer &) = delete;

  /// @brief Renders one frame.
  bool drawFrame(VkRenderPass renderPass, VkPipeline graphicsPipeline,
                 VkPipelineLayout pipelineLayout, VkBuffer vertexBuffer,
                 VkBuffer indexBuffer, const LoadedModel &model,
                 const std::vector<VkDescriptorSet> &descriptorSets,
                 const Camera &camera, VkExtent2D swapChainExtent,
                 std::vector<void *> &uniformBuffersMapped,
                 bool &framebufferResized);

private:
  VulkanContext *context_;
  SwapChain *swapChain_;

  VkCommandPool commandPool_ = VK_NULL_HANDLE;
  std::vector<VkCommandBuffer> commandBuffers_;

  std::vector<VkSemaphore> imageAvailableSemaphores_;
  std::vector<VkSemaphore> renderFinishedSemaphores_;
  std::vector<VkFence> inFlightFences_;
  uint32_t currentFrame_ = 0;

  static constexpr int MAX_FRAMES_IN_FLIGHT = 2;

  void createCommandPool();
  void createCommandBuffers();
  void createSyncObjects();
  void cleanup();

  void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex,
                           VkRenderPass renderPass, VkPipeline graphicsPipeline,
                           VkPipelineLayout pipelineLayout,
                           VkBuffer vertexBuffer, VkBuffer indexBuffer,
                           const LoadedModel &model,
                           const std::vector<VkDescriptorSet> &descriptorSets,
                           VkExtent2D swapChainExtent);

  void updateUniformBuffer(uint32_t currentImage, const Camera &camera,
                           VkExtent2D swapChainExtent,
                           std::vector<void *> &uniformBuffersMapped);
};
