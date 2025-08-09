#pragma once

#include "CommandPool.hpp"
#include "render/engines/vulkan/Device.hpp"
#include "render/engines/vulkan/Pipeline.hpp"
#include "render/engines/vulkan/RenderPass.hpp"
#include "render/engines/vulkan/Swapchain.hpp"
#include <vulkan/vulkan_core.h>

class CommandBuffer {
    public:
        CommandBuffer(const Device &device, const Swapchain &swapchain, const RenderPass &renderPass,
            const Pipeline &pipeline, const CommandPool &commandPool);
        ~CommandBuffer();

        void record(const VkFramebuffer framebuffer);

    private:
        const Device &device;
        const Swapchain &swapchain;
        const RenderPass &renderPass;
        const Pipeline &pipeline;
        const CommandPool &commandPool;
        VkCommandBuffer commandBuffer = VK_NULL_HANDLE;
};