#pragma once

#include "CommandPool.hpp"
#include "Device.hpp"
#include "Pipeline.hpp"
#include "Swapchain.hpp"
#include <vulkan/vulkan_core.h>

class CommandBuffers {
    public:
        CommandBuffers(const Device &device, const Swapchain &swapchain, const Pipeline &pipeline,
            const CommandPool &commandPool, uint32_t count);
        ~CommandBuffers();

        const VkCommandBuffer &getVkCommandBuffer(uint32_t index) const noexcept;

        void reset(uint32_t index);
        void record(uint32_t index, VkFramebuffer framebuffer);

    private:
        const Device &device;
        const Swapchain &swapchain;
        const Pipeline &pipeline;
        const CommandPool &commandPool;
        std::vector<VkCommandBuffer> commandBuffers{};
};