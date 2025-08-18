#pragma once

#include "Device.hpp"
#include "Pipeline.hpp"
#include "Swapchain.hpp"
#include <vulkan/vulkan_core.h>

class CommandBuffers {
    public:
        CommandBuffers(const Device &device, const Swapchain &swapchain, const Pipeline &pipeline, uint32_t count);
        ~CommandBuffers();

        VkCommandBuffer getVkCommandBuffer(uint32_t index) const noexcept;

        void record(uint32_t index, uint32_t imageIndex);

    private:
        const Device &device;
        const Swapchain &swapchain;
        const Pipeline &pipeline;
        std::vector<VkCommandBuffer> commandBuffers{};
};