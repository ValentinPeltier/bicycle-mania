#include "ImageViews.hpp"
#include "render/engines/vulkan/Swapchain.hpp"
#include <stdexcept>
#include <vulkan/vulkan_core.h>

ImageViews::ImageViews(const Device &device, const Swapchain &swapchain)
    : device(device),
      swapchain(swapchain) {

    VkComponentMapping components{};
    components.r = VkComponentSwizzle::VK_COMPONENT_SWIZZLE_IDENTITY;
    components.g = VkComponentSwizzle::VK_COMPONENT_SWIZZLE_IDENTITY;
    components.b = VkComponentSwizzle::VK_COMPONENT_SWIZZLE_IDENTITY;
    components.a = VkComponentSwizzle::VK_COMPONENT_SWIZZLE_IDENTITY;

    VkImageSubresourceRange subresourceRange{};
    subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    subresourceRange.baseMipLevel = 0;
    subresourceRange.levelCount = 1;
    subresourceRange.baseArrayLayer = 0;
    subresourceRange.layerCount = 1;

    this->imageViews.resize(this->swapchain.getImages().size());
    for (uint32_t i = 0; i < this->swapchain.getImages().size(); ++i) {
        VkImageViewCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.image = this->swapchain.getImages()[i];
        createInfo.viewType = VkImageViewType::VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format = this->swapchain.getFormat();
        createInfo.components = components;
        createInfo.subresourceRange = subresourceRange;

        if (vkCreateImageView(this->device.getVkDevice(), &createInfo, nullptr, &this->imageViews[i]) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create image view.");
        }
    }
}

ImageViews::~ImageViews() {
    for (auto &imageView : this->imageViews) {
        vkDestroyImageView(this->device.getVkDevice(), imageView, nullptr);
    }
}