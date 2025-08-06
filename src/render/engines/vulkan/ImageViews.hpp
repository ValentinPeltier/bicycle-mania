#pragma once

#include "render/engines/vulkan/Device.hpp"
#include "render/engines/vulkan/Swapchain.hpp"
#include <vector>
#include <vulkan/vulkan_core.h>
class ImageViews {
    public:
        ImageViews(const Device &device, const Swapchain &swapchain);
        ~ImageViews();

    private:
        const Device &device;
        const Swapchain &swapchain;
        std::vector<VkImageView> imageViews{};
};