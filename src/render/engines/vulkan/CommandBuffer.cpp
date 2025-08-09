#include "CommandBuffer.hpp"
#include "CommandPool.hpp"
#include "RenderPass.hpp"
#include "render/engines/vulkan/Pipeline.hpp"
#include <stdexcept>
#include <vulkan/vulkan_core.h>

CommandBuffer::CommandBuffer(const Device &device, const Swapchain &swapchain, const RenderPass &renderPass,
    const Pipeline &pipeline, const CommandPool &commandPool)
    : device(device),
      swapchain(swapchain),
      renderPass(renderPass),
      pipeline(pipeline),
      commandPool(commandPool) {
    VkCommandBufferAllocateInfo allocateInfo{};
    allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocateInfo.commandPool = this->commandPool.getVkCommandPool();
    allocateInfo.commandBufferCount = 1;
    allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

    if (vkAllocateCommandBuffers(this->device.getVkDevice(), &allocateInfo, &this->commandBuffer) != VK_SUCCESS) {
        throw std::runtime_error("Failed to allocate command buffers.");
    }
}

CommandBuffer::~CommandBuffer() {
    vkFreeCommandBuffers(this->device.getVkDevice(), this->commandPool.getVkCommandPool(), 1, &this->commandBuffer);
}

void CommandBuffer::record(const VkFramebuffer framebuffer) {
    VkCommandBufferBeginInfo commandBufferInfo{};
    commandBufferInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (vkBeginCommandBuffer(this->commandBuffer, &commandBufferInfo) != VK_SUCCESS) {
        throw std::runtime_error("Failed to begin command buffer.");
    }

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = this->renderPass.getVkRenderPass();
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = this->swapchain.getExtent();
    renderPassInfo.framebuffer = framebuffer;
    VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;
    vkCmdBeginRenderPass(this->commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(this->commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, this->pipeline.getVkPipeline());

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = this->swapchain.getExtent().width;
    viewport.height = this->swapchain.getExtent().height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(this->commandBuffer, 0, 1, &viewport);

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = this->swapchain.getExtent();
    vkCmdSetScissor(this->commandBuffer, 0, 1, &scissor);

    vkCmdDraw(this->commandBuffer, 3, 1, 0, 0);

    vkCmdEndRenderPass(this->commandBuffer);

    if (vkEndCommandBuffer(this->commandBuffer) != VK_SUCCESS) {
        throw std::runtime_error("Failed to end command buffer.");
    }
}