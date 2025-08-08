#pragma once

#include "Device.hpp"
#include "Swapchain.hpp"
#include <vulkan/vulkan_core.h>

class RenderPass {
    public:
        RenderPass(const Device &device, const Swapchain &swapchain);
        ~RenderPass();

        VkRenderPass getVkRenderPass() const noexcept;

    private:
        const Device &device;
        const Swapchain &swapchain;
        VkRenderPass renderPass = VK_NULL_HANDLE;
};