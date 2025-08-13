#include "Swapchain.hpp"
#include "Device.hpp"
#include "Surface.hpp"
#include "core/Logger.hpp"
#include <algorithm>
#include <cstdint>
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
    this->imageViews = this->createImageViews();
    this->renderPass = this->createRenderPass();
    this->framebuffers = this->createFramebuffers();
}

Swapchain::~Swapchain() {
    for (auto &framebuffer : this->framebuffers) {
        vkDestroyFramebuffer(this->device.getVkDevice(), framebuffer, nullptr);
    }

    vkDestroyRenderPass(this->device.getVkDevice(), this->renderPass, nullptr);

    for (auto &imageView : this->imageViews) {
        vkDestroyImageView(this->device.getVkDevice(), imageView, nullptr);
    }

    vkDestroySwapchainKHR(this->device.getVkDevice(), this->swapchain, nullptr);
}

VkSwapchainKHR Swapchain::getVkSwapchain() const noexcept {
    return this->swapchain;
}

const VkExtent2D &Swapchain::getExtent() const noexcept {
    return this->extent;
}

uint32_t Swapchain::getNextImageIndex(VkSemaphore signalSemaphore) const {
    uint32_t imageIndex;
    if (vkAcquireNextImageKHR(this->device.getVkDevice(), this->swapchain, UINT64_MAX, signalSemaphore, VK_NULL_HANDLE,
            &imageIndex) != VK_SUCCESS) {
        throw std::runtime_error("Failed to acquire next image.");
    }
    return imageIndex;
}

VkFramebuffer Swapchain::getFramebuffer(uint32_t index) const {
    return this->framebuffers[index];
}

VkRenderPass Swapchain::getVkRenderPass() const noexcept {
    return this->renderPass;
}

uint32_t Swapchain::getImageCount() const noexcept {
    return this->images.size();
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

std::vector<VkImageView> Swapchain::createImageViews() const {
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

    std::vector<VkImageView> imageViews(this->images.size());
    for (uint32_t i = 0; i < this->images.size(); ++i) {
        VkImageViewCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.image = this->images[i];
        createInfo.viewType = VkImageViewType::VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format = this->format;
        createInfo.components = components;
        createInfo.subresourceRange = subresourceRange;

        if (vkCreateImageView(this->device.getVkDevice(), &createInfo, nullptr, &imageViews[i]) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create image view.");
        }
    }

    return imageViews;
}

VkRenderPass Swapchain::createRenderPass() const {
    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = this->format;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachmentReference{};
    colorAttachmentReference.attachment = 0;
    colorAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentReference;
    VkSubpassDependency subpassDependency{};
    subpassDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    subpassDependency.dstSubpass = 0;
    subpassDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    subpassDependency.srcAccessMask = 0;
    subpassDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    subpassDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    VkRenderPassCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    createInfo.attachmentCount = 1;
    createInfo.pAttachments = &colorAttachment;
    createInfo.subpassCount = 1;
    createInfo.pSubpasses = &subpass;
    createInfo.dependencyCount = 1;
    createInfo.pDependencies = &subpassDependency;

    VkRenderPass renderPass = VK_NULL_HANDLE;
    if (vkCreateRenderPass(this->device.getVkDevice(), &createInfo, nullptr, &renderPass) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create render pass.");
    }

    return renderPass;
}

std::vector<VkFramebuffer> Swapchain::createFramebuffers() const {
    std::vector<VkFramebuffer> framebuffers(this->imageViews.size());

    for (uint32_t i = 0; i < this->imageViews.size(); i++) {
        std::vector<VkImageView> attachments{
            this->imageViews[i],
        };

        VkFramebufferCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        createInfo.renderPass = this->renderPass;
        createInfo.attachmentCount = attachments.size();
        createInfo.pAttachments = attachments.data();
        createInfo.width = this->extent.width;
        createInfo.height = this->extent.height;
        createInfo.layers = 1;

        if (vkCreateFramebuffer(this->device.getVkDevice(), &createInfo, nullptr, &framebuffers[i]) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create the framebuffers.");
        }
    }

    return framebuffers;
}
