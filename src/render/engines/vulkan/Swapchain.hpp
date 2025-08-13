#pragma once

class Swapchain;

#include "Device.hpp"
#include "Surface.hpp"
#include <cstdint>
#include <vector>
#include <vulkan/vulkan_core.h>

struct SwapchainDetails {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;
};

class Swapchain {
    public:
        Swapchain(const Window &window, const Surface &surface, const Device &device);
        ~Swapchain();

        VkSwapchainKHR getVkSwapchain() const noexcept;
        const VkExtent2D &getExtent() const noexcept;
        uint32_t getNextImageIndex(VkSemaphore signalSemaphore) const;
        VkFramebuffer getFramebuffer(uint32_t index) const;
        VkRenderPass getVkRenderPass() const noexcept;
        uint32_t getImageCount() const noexcept;

        static uint32_t rate(VkPhysicalDevice physicalDevice, const Surface &surface);

    private:
        const Window &window;
        const Surface &surface;
        const Device &device;
        VkSwapchainKHR swapchain = VK_NULL_HANDLE;
        VkFormat format{};
        VkExtent2D extent{};
        std::vector<VkImage> images{};
        std::vector<VkImageView> imageViews{};
        std::vector<VkFramebuffer> framebuffers{};
        VkRenderPass renderPass = VK_NULL_HANDLE;

        static SwapchainDetails getDetails(VkPhysicalDevice physicalDevice, const Surface &surface);

        VkSurfaceFormatKHR chooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &surfaceFormats) const;
        VkPresentModeKHR choosePresentMode(const std::vector<VkPresentModeKHR> &presentModes) const;
        VkExtent2D chooseExtent(const VkSurfaceCapabilitiesKHR &capabilities) const;
        uint32_t chooseImageCount(const VkSurfaceCapabilitiesKHR &capabilities) const;

        std::vector<VkImage> fetchImages() const;

        std::vector<VkImageView> createImageViews() const;
        VkRenderPass createRenderPass() const;
        std::vector<VkFramebuffer> createFramebuffers() const;
};