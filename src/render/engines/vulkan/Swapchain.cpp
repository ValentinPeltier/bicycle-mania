#include "Swapchain.hpp"
#include "Device.hpp"
#include "Surface.hpp"
#include <algorithm>
#include <limits>
#include <stdexcept>
#include <vector>
#include <vulkan/vulkan_core.h>

Swapchain::Swapchain(const Window &window, const Surface &surface, const Device &device)
    : window(window),
      surface(surface),
      device(device) {
    SwapchainDetails details = Swapchain::getDetails(this->device.getVkPhysicalDevice(), this->surface);
    VkSurfaceFormatKHR surfaceFormat = this->chooseSurfaceFormat(details.formats);
    VkPresentModeKHR presentMode = this->choosePresentMode(details.presentModes);
    VkExtent2D extent = this->chooseExtent(details.capabilities);

    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = this->surface.getVkSurface();
    createInfo.minImageCount = this->chooseImageCount(details.capabilities);
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    createInfo.preTransform = details.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = VK_NULL_HANDLE;

    QueueFamilyIndices queueFamilyIndices = this->device.getQueueFamilyIndices();
    if (queueFamilyIndices.graphics == queueFamilyIndices.present) {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    } else {
        uint32_t queueFamilyIndicesArray[] = {
            queueFamilyIndices.graphics.value(),
            queueFamilyIndices.present.value(),
        };
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndicesArray;
    }

    if (vkCreateSwapchainKHR(this->device.getVkDevice(), &createInfo, nullptr, &this->swapchain) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create the swapchain.");
    }

    this->format = surfaceFormat.format;
    this->extent = extent;
    this->images = this->fetchImages();
}

Swapchain::~Swapchain() {
    vkDestroySwapchainKHR(this->device.getVkDevice(), this->swapchain, nullptr);
}

const std::vector<VkImage> &Swapchain::getImages() const {
    return this->images;
}

const VkFormat &Swapchain::getFormat() const {
    return this->format;
}

uint32_t Swapchain::rate(VkPhysicalDevice physicalDevice, const Surface &surface) {
    SwapchainDetails details = Swapchain::getDetails(physicalDevice, surface);

    // We need at least one format
    if (details.formats.empty()) {
        return 0;
    }

    // We need at least one present mode
    if (details.presentModes.empty()) {
        return 0;
    }

    return 1;
}

SwapchainDetails Swapchain::getDetails(VkPhysicalDevice physicalDevice, const Surface &surface) {
    SwapchainDetails details{};

    // Get surface capabilities
    if (vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface.getVkSurface(), &details.capabilities) !=
        VK_SUCCESS) {
        throw std::runtime_error("Failed to get the physical device surface capabilities.");
    }

    // Get surface formats
    uint32_t surfaceFormatCount;
    if (vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface.getVkSurface(), &surfaceFormatCount, nullptr) !=
        VK_SUCCESS) {
        throw std::runtime_error("Failed to get the physical device surface formats.");
    }
    details.formats.resize(surfaceFormatCount);
    if (vkGetPhysicalDeviceSurfaceFormatsKHR(
            physicalDevice, surface.getVkSurface(), &surfaceFormatCount, details.formats.data()) != VK_SUCCESS) {
        throw std::runtime_error("Failed to get the physical device surface formats.");
    }

    // Get present modes
    uint32_t presentModeCount;
    if (vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface.getVkSurface(), &presentModeCount, nullptr) !=
        VK_SUCCESS) {
        throw std::runtime_error("Failed to get the physical device present modes.");
    }
    if (presentModeCount > 0) {
        details.presentModes.resize(presentModeCount);
        if (vkGetPhysicalDeviceSurfacePresentModesKHR(
                physicalDevice, surface.getVkSurface(), &presentModeCount, details.presentModes.data()) != VK_SUCCESS) {
            throw std::runtime_error("Failed to get the physical device present modes.");
        }
    }

    return details;
}

VkSurfaceFormatKHR Swapchain::chooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &surfaceFormats) const {
    // TODO rate each surface format and return the best one
    for (const auto &surfaceFormat : surfaceFormats) {
        if (surfaceFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
            surfaceFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return surfaceFormat;
        }
    }

    return surfaceFormats[0];
}

VkPresentModeKHR Swapchain::choosePresentMode(const std::vector<VkPresentModeKHR> &presentModes) const {
    // Prefer VK_PRESENT_MODE_MAILBOX_KHR
    if (std::find(presentModes.begin(), presentModes.end(), VK_PRESENT_MODE_MAILBOX_KHR) != presentModes.end()) {
        return VK_PRESENT_MODE_MAILBOX_KHR;
    }

    // Defaults to VK_PRESENT_MODE_FIFO_KHR
    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D Swapchain::chooseExtent(const VkSurfaceCapabilitiesKHR &capabilities) const {
    // If Vulkan already fixed the extent, just keep it
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
        return capabilities.currentExtent;
    }

    // Otherwise, set it to the resolution of the window
    return {
        std::clamp(this->window.getFramebufferSize().width, capabilities.minImageExtent.width,
            capabilities.maxImageExtent.width),
        std::clamp(this->window.getFramebufferSize().height, capabilities.minImageExtent.height,
            capabilities.maxImageExtent.height),
    };
}

uint32_t Swapchain::chooseImageCount(const VkSurfaceCapabilitiesKHR &capabilities) const {
    uint32_t imageCount = capabilities.minImageCount + 1;

    if (capabilities.maxImageCount != 0 && imageCount > capabilities.maxImageCount) {
        return capabilities.maxImageCount;
    }

    return imageCount;
}

std::vector<VkImage> Swapchain::fetchImages() const {
    uint32_t imageCount;
    if (vkGetSwapchainImagesKHR(this->device.getVkDevice(), this->swapchain, &imageCount, nullptr) != VK_SUCCESS) {
        throw std::runtime_error("Failed to fetch swapchain images.");
    }
    std::vector<VkImage> images(imageCount);
    if (vkGetSwapchainImagesKHR(this->device.getVkDevice(), this->swapchain, &imageCount, images.data()) !=
        VK_SUCCESS) {
        throw std::runtime_error("Failed to fetch swapchain images.");
    }

    return images;
}