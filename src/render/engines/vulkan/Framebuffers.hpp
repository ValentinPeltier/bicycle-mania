#pragma once

#include "ImageViews.hpp"
#include "RenderPass.hpp"
#include "Swapchain.hpp"
#include <vector>
#include <vulkan/vulkan_core.h>

class Framebuffers {
    public:
        Framebuffers(const Device &device, const Swapchain &swapchain, const ImageViews &imageViews,
            const RenderPass &renderPass);
        ~Framebuffers();

    private:
        const Device &device;
        const Swapchain &swapchain;
        const ImageViews &imageViews;
        const RenderPass &renderPass;
        std::vector<VkFramebuffer> framebuffers{};
};