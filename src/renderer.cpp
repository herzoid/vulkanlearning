#include "renderer.hpp"
#include "UBO.hpp"
#include "camera.hpp"
#include "model.hpp"
#include "swapchain.hpp"
#include "vulkan_context.hpp"

#include <array>
#include <chrono>
#include <cstring>
#include <glm/glm.hpp>
#include <stdexcept>

Renderer::Renderer(VulkanContext *context, SwapChain *swapChain)
    : context_(context), swapChain_(swapChain) {
  createCommandPool();
  createCommandBuffers();
  createSyncObjects();
}

Renderer::~Renderer() { cleanup(); }

void Renderer::createCommandPool() {
  VulkanContext::QueueFamilyIndices queueFamilyIndices =
      context_->findQueueFamilies(context_->getPhysicalDevice());

  VkCommandPoolCreateInfo poolInfo{};
  poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
  poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

  if (vkCreateCommandPool(context_->getDevice(), &poolInfo, nullptr,
                          &commandPool_) != VK_SUCCESS) {
    throw std::runtime_error("failed to create graphics command pool!");
  }
}

void Renderer::createCommandBuffers() {
  commandBuffers_.resize(MAX_FRAMES_IN_FLIGHT);

  VkCommandBufferAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocInfo.commandPool = commandPool_;
  allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocInfo.commandBufferCount = (uint32_t)commandBuffers_.size();

  if (vkAllocateCommandBuffers(context_->getDevice(), &allocInfo,
                               commandBuffers_.data()) != VK_SUCCESS) {
    throw std::runtime_error("failed to allocate command buffers!");
  }
}

void Renderer::createSyncObjects() {
  imageAvailableSemaphores_.resize(swapChain_->getImages().size());
  renderFinishedSemaphores_.resize(swapChain_->getImages().size());
  inFlightFences_.resize(MAX_FRAMES_IN_FLIGHT);

  VkSemaphoreCreateInfo semaphoreInfo{};
  semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

  VkFenceCreateInfo fenceInfo{};
  fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

  for (size_t i = 0; i < swapChain_->getImages().size(); i++) {
    if (vkCreateSemaphore(context_->getDevice(), &semaphoreInfo, nullptr,
                          &imageAvailableSemaphores_[i]) != VK_SUCCESS ||
        vkCreateSemaphore(context_->getDevice(), &semaphoreInfo, nullptr,
                          &renderFinishedSemaphores_[i]) != VK_SUCCESS) {
      throw std::runtime_error(
          "failed to create semaphores for a swapchain images!");
    }
  }

  for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
    if (vkCreateFence(context_->getDevice(), &fenceInfo, nullptr,
                      &inFlightFences_[i]) != VK_SUCCESS) {
      throw std::runtime_error("failed to create fences for a frame!");
    }
  }
}

void Renderer::cleanup() {
  for (size_t i = 0; i < swapChain_->getImages().size(); i++) {
    vkDestroySemaphore(context_->getDevice(), renderFinishedSemaphores_[i],
                       nullptr);
    vkDestroySemaphore(context_->getDevice(), imageAvailableSemaphores_[i],
                       nullptr);
  }

  for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
    vkDestroyFence(context_->getDevice(), inFlightFences_[i], nullptr);
  }

  vkDestroyCommandPool(context_->getDevice(), commandPool_, nullptr);
}

void Renderer::recordCommandBuffer(
    VkCommandBuffer commandBuffer, uint32_t imageIndex, VkRenderPass renderPass,
    VkPipeline graphicsPipeline, VkPipelineLayout pipelineLayout,
    VkBuffer vertexBuffer, VkBuffer indexBuffer, const LoadedModel &model,
    const std::vector<VkDescriptorSet> &descriptorSets,
    VkExtent2D swapChainExtent) {
  VkCommandBufferBeginInfo beginInfo{};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

  if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
    throw std::runtime_error("failed to begin recording command buffer!");
  }

  VkRenderPassBeginInfo renderPassInfo{};
  renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  renderPassInfo.renderPass = renderPass;
  renderPassInfo.framebuffer = swapChain_->getFramebuffers()[imageIndex];
  renderPassInfo.renderArea.offset = {0, 0};
  renderPassInfo.renderArea.extent = swapChain_->getExtent();

  std::array<VkClearValue, 2> clearValues{};
  clearValues[0].color = {{0.0f, 0.0f, 0.0f, 1.0f}};
  clearValues[1].depthStencil = {1.0f, 0};
  renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
  renderPassInfo.pClearValues = clearValues.data();

  vkCmdBeginRenderPass(commandBuffer, &renderPassInfo,
                       VK_SUBPASS_CONTENTS_INLINE);

  vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                    graphicsPipeline);

  VkViewport viewport{};
  viewport.x = 0.0f;
  viewport.y = 0.0f;
  viewport.width = (float)swapChain_->getExtent().width;
  viewport.height = (float)swapChain_->getExtent().height;
  viewport.minDepth = 0.0f;
  viewport.maxDepth = 1.0f;
  vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

  VkRect2D scissor{};
  scissor.offset = {0, 0};
  scissor.extent = swapChain_->getExtent();
  vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

  VkBuffer vertexBuffers[] = {vertexBuffer};
  VkDeviceSize offsets[] = {0};
  vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

  vkCmdBindIndexBuffer(commandBuffer, indexBuffer, 0, VK_INDEX_TYPE_UINT32);

  for (const auto &mesh : model.meshes) {
    if (mesh.textureIndex < 0 ||
        mesh.textureIndex >= static_cast<int>(model.textures.size()))
      continue;
    VkDescriptorSet sets[] = {descriptorSets[mesh.textureIndex]};
    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                            pipelineLayout, 0, 1, sets, 0, nullptr);

    vkCmdDrawIndexed(commandBuffer, mesh.indexCount, 1, mesh.indexOffset, 0, 0);
  }

  vkCmdEndRenderPass(commandBuffer);

  if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
    throw std::runtime_error("failed to record command buffer!");
  }
}

bool Renderer::drawFrame(VkRenderPass renderPass, VkPipeline graphicsPipeline,
                         VkPipelineLayout pipelineLayout, VkBuffer vertexBuffer,
                         VkBuffer indexBuffer, const LoadedModel &model,
                         const std::vector<VkDescriptorSet> &descriptorSets,
                         const Camera &camera, VkExtent2D swapChainExtent,
                         std::vector<void *> &uniformBuffersMapped,
                         bool &framebufferResized) {
  vkWaitForFences(context_->getDevice(), 1, &inFlightFences_[currentFrame_],
                  VK_TRUE, UINT64_MAX);

  uint32_t imageIndex;
  VkResult result = vkAcquireNextImageKHR(
      context_->getDevice(), swapChain_->getSwapChain(), UINT64_MAX,
      imageAvailableSemaphores_[currentFrame_], VK_NULL_HANDLE, &imageIndex);

  if (result == VK_ERROR_OUT_OF_DATE_KHR) {
    return true;
  } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
    throw std::runtime_error("failed to acquire swap chain image!");
  }

  updateUniformBuffer(currentFrame_, camera, swapChainExtent,
                      uniformBuffersMapped);

  vkResetFences(context_->getDevice(), 1, &inFlightFences_[currentFrame_]);

  vkResetCommandBuffer(commandBuffers_[currentFrame_], 0);

  recordCommandBuffer(commandBuffers_[currentFrame_], imageIndex, renderPass,
                      graphicsPipeline, pipelineLayout, vertexBuffer,
                      indexBuffer, model, descriptorSets, swapChainExtent);

  VkSubmitInfo submitInfo{};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

  VkSemaphore waitSemaphores[] = {imageAvailableSemaphores_[currentFrame_]};
  VkPipelineStageFlags waitStages[] = {
      VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
  submitInfo.waitSemaphoreCount = 1;
  submitInfo.pWaitSemaphores = waitSemaphores;
  submitInfo.pWaitDstStageMask = waitStages;

  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &commandBuffers_[currentFrame_];

  VkSemaphore signalSemaphores[] = {renderFinishedSemaphores_[imageIndex]};
  submitInfo.signalSemaphoreCount = 1;
  submitInfo.pSignalSemaphores = signalSemaphores;

  if (vkQueueSubmit(context_->getGraphicsQueue(), 1, &submitInfo,
                    inFlightFences_[currentFrame_]) != VK_SUCCESS) {
    throw std::runtime_error("failed to submit draw command buffer!");
  }

  VkPresentInfoKHR presentInfo{};
  presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

  presentInfo.waitSemaphoreCount = 1;
  presentInfo.pWaitSemaphores = signalSemaphores;

  VkSwapchainKHR swapChains[] = {swapChain_->getSwapChain()};
  presentInfo.swapchainCount = 1;
  presentInfo.pSwapchains = swapChains;

  presentInfo.pImageIndices = &imageIndex;

  result = vkQueuePresentKHR(context_->getPresentQueue(), &presentInfo);

  if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR ||
      framebufferResized) {
    framebufferResized = false;
    return true;
  } else if (result != VK_SUCCESS) {
    throw std::runtime_error("failed to present swap chain image!");
  }

  currentFrame_ = (currentFrame_ + 1) % MAX_FRAMES_IN_FLIGHT;
  return false;
}

void Renderer::updateUniformBuffer(uint32_t currentImage, const Camera &camera,
                                   VkExtent2D swapChainExtent,
                                   std::vector<void *> &uniformBuffersMapped) {
  static auto startTime = std::chrono::high_resolution_clock::now();

  auto currentTime = std::chrono::high_resolution_clock::now();
  float time = std::chrono::duration<float, std::chrono::seconds::period>(
                   currentTime - startTime)
                   .count();

  UniformBufferObject ubo{};

  ubo.model = glm::mat4(1.0f);

  ubo.view = camera.getViewMatrix();

  ubo.proj = camera.getProjectionMatrix(swapChain_->getExtent().width /
                                        (float)swapChain_->getExtent().height);

  memcpy(uniformBuffersMapped[currentImage], &ubo, sizeof(ubo));
}
