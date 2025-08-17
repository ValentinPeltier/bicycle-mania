#include "CommandBuffers.hpp"
#include "Pipeline.hpp"
#include <stdexcept>
#include <vulkan/vulkan_core.h>

CommandBuffers::CommandBuffers(
    const Device &device, const Swapchain &swapchain, const Pipeline &pipeline, uint32_t count)
    : device(device),
      swapchain(swapchain),
      pipeline(pipeline) {
    this->commandBuffers.resize(count);

    VkCommandBufferAllocateInfo allocateInfo{};
    allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocateInfo.commandPool = this->device.getGraphicsVkCommandPool();
    allocateInfo.commandBufferCount = count;
    allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

    if (vkAllocateCommandBuffers(this->device.getVkDevice(), &allocateInfo, this->commandBuffers.data()) !=
        VK_SUCCESS) {
        throw std::runtime_error("Failed to allocate command buffers.");
    }
}

CommandBuffers::~CommandBuffers() {
    vkFreeCommandBuffers(this->device.getVkDevice(), this->device.getGraphicsVkCommandPool(),
        this->commandBuffers.size(), this->commandBuffers.data());
}

VkCommandBuffer CommandBuffers::getVkCommandBuffer(uint32_t index) const noexcept {
    return this->commandBuffers[index];
}

void CommandBuffers::record(uint32_t index, VkFramebuffer framebuffer) {
    if (vkResetCommandBuffer(this->commandBuffers[index], 0) != VK_SUCCESS) {
        throw std::runtime_error("Failed to reset the command buffer.");
    }

    VkCommandBufferBeginInfo commandBufferInfo{};
    commandBufferInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (vkBeginCommandBuffer(this->commandBuffers[index], &commandBufferInfo) != VK_SUCCESS) {
        throw std::runtime_error("Failed to begin command buffer.");
    }

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = this->swapchain.getVkRenderPass();
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = this->swapchain.getExtent();
    renderPassInfo.framebuffer = framebuffer;
    VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;
    vkCmdBeginRenderPass(this->commandBuffers[index], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(this->commandBuffers[index], VK_PIPELINE_BIND_POINT_GRAPHICS, this->pipeline.getVkPipeline());

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = this->swapchain.getExtent().width;
    viewport.height = this->swapchain.getExtent().height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(this->commandBuffers[index], 0, 1, &viewport);

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = this->swapchain.getExtent();
    vkCmdSetScissor(this->commandBuffers[index], 0, 1, &scissor);

    vkCmdDraw(this->commandBuffers[index], 3, 1, 0, 0);

    vkCmdEndRenderPass(this->commandBuffers[index]);

    if (vkEndCommandBuffer(this->commandBuffers[index]) != VK_SUCCESS) {
        throw std::runtime_error("Failed to end command buffer.");
    }
}