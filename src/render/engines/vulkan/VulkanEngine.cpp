#include "VulkanEngine.hpp"
#include <cstdint>
#include <stdexcept>
#include <vulkan/vulkan_core.h>

VulkanEngine::VulkanEngine(const Window &window)
    : instance(),
      surface(this->instance, window),
      device(this->instance, this->surface),
      swapchain(window, this->surface, this->device),
      pipeline(this->device, this->swapchain),
      commandBuffers(this->device, this->swapchain, this->pipeline, this->MAX_FRAMES_WAITING) {

    // Create synchronization objects
    this->imageAvailableSemaphores.resize(this->MAX_FRAMES_WAITING);
    this->renderFinishedSemaphores.resize(this->swapchain.getImageCount());
    this->framePresentedFences.resize(this->MAX_FRAMES_WAITING);

    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (uint32_t i = 0; i < this->MAX_FRAMES_WAITING; ++i) {
        if (vkCreateSemaphore(this->device.getVkDevice(), &semaphoreInfo, nullptr,
                &this->imageAvailableSemaphores[i]) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create semaphore.");
        }
    }

    for (uint32_t i = 0; i < this->swapchain.getImageCount(); ++i) {
        if (vkCreateSemaphore(this->device.getVkDevice(), &semaphoreInfo, nullptr,
                &this->renderFinishedSemaphores[i]) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create semaphore.");
        }
    }

    for (uint32_t i = 0; i < this->MAX_FRAMES_WAITING; ++i) {
        if (vkCreateFence(this->device.getVkDevice(), &fenceInfo, nullptr, &this->framePresentedFences[i]) !=
            VK_SUCCESS) {
            throw std::runtime_error("Failed to create fence.");
        }
    }
}

VulkanEngine::~VulkanEngine() {
    vkDeviceWaitIdle(this->device.getVkDevice());

    for (uint32_t i = 0; i < this->MAX_FRAMES_WAITING; ++i) {
        vkDestroyFence(this->device.getVkDevice(), this->framePresentedFences[i], nullptr);
    }
    for (uint32_t i = 0; i < this->swapchain.getImageCount(); ++i) {
        vkDestroySemaphore(this->device.getVkDevice(), this->renderFinishedSemaphores[i], nullptr);
    }
    for (uint32_t i = 0; i < this->MAX_FRAMES_WAITING; ++i) {
        vkDestroySemaphore(this->device.getVkDevice(), this->imageAvailableSemaphores[i], nullptr);
    }
}

void VulkanEngine::draw() {
    // Wait for previous frame to be presented
    if (vkWaitForFences(this->device.getVkDevice(), 1, &this->framePresentedFences[this->frameIndex], VK_TRUE,
            UINT64_MAX) != VK_SUCCESS) {
        throw std::runtime_error("Failed to wait for fence.");
    }
    if (vkResetFences(this->device.getVkDevice(), 1, &this->framePresentedFences[this->frameIndex]) != VK_SUCCESS) {
        throw std::runtime_error("Failed to reset fence.");
    }

    uint32_t imageIndex;
    if (vkAcquireNextImageKHR(this->device.getVkDevice(), this->swapchain.getVkSwapchain(), UINT64_MAX,
            this->imageAvailableSemaphores[this->frameIndex], VK_NULL_HANDLE, &imageIndex) != VK_SUCCESS) {
        throw std::runtime_error("Failed to acquire next image.");
    }

    // Record command buffer
    this->commandBuffers.record(this->frameIndex, this->swapchain.getVkFramebuffer(imageIndex));

    // Submit draw command buffer
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    VkSemaphore waitSemaphores[] = {this->imageAvailableSemaphores[this->frameIndex]};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    const VkCommandBuffer commandBuffer = this->commandBuffers.getVkCommandBuffer(this->frameIndex);
    submitInfo.pCommandBuffers = &commandBuffer;
    VkSemaphore signalSemaphores[] = {this->renderFinishedSemaphores[imageIndex]};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;
    if (vkQueueSubmit(this->device.getGraphicsVkQueue(), 1, &submitInfo,
            this->framePresentedFences[this->frameIndex]) != VK_SUCCESS) {
        throw std::runtime_error("Failed to submit draw command buffer.");
    }

    // Present the image to the swapchain
    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;
    VkSwapchainKHR swapchains[] = {this->swapchain.getVkSwapchain()};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapchains;
    presentInfo.pImageIndices = &imageIndex;

    if (vkQueuePresentKHR(this->device.getPresentVkQueue(), &presentInfo) != VK_SUCCESS) {
        throw std::runtime_error("Unable to present the image.");
    }

    this->frameIndex = (this->frameIndex + 1) % this->MAX_FRAMES_WAITING;
}