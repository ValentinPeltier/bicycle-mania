#pragma once

#include "Device.hpp"
#include "Swapchain.hpp"
#include <cstdint>
#include <vector>
#include <vulkan/vulkan_core.h>
class ImageViews {
    public:
        ImageViews(const Device &device, const Swapchain &swapchain);
        ~ImageViews();

        uint32_t getSize() const noexcept;
        const std::vector<VkImageView> &getVkImageViews() const noexcept;

    private:
        const Device &device;
        const Swapchain &swapchain;
        std::vector<VkImageView> imageViews{};
};