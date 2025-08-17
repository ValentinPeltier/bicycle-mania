#pragma once

#include "Instance.hpp"
#include "Surface.hpp"
#include <array>
#include <cstdint>
#include <optional>
#include <vulkan/vulkan_core.h>

class Device {
    public:
        Device(const Instance &instance, const Surface &surface);
        ~Device();

        VkDevice getVkDevice() const noexcept;
        VkPhysicalDevice getVkPhysicalDevice() const noexcept;

        // Queues
        uint32_t getGraphicsQueueFamilyIndex() const noexcept;
        uint32_t getPresentQueueFamilyIndex() const noexcept;
        VkQueue getGraphicsVkQueue() const noexcept;
        VkQueue getPresentVkQueue() const noexcept;

        // Command pools
        VkCommandPool getGraphicsVkCommandPool() const noexcept;

    private:
        const Instance &instance;
        const Surface &surface;
        VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
        VkDevice device = VK_NULL_HANDLE;

        // Queues
        uint32_t graphicsQueueFamilyIndex;
        uint32_t presentQueueFamilyIndex;
        VkQueue graphicsQueue = VK_NULL_HANDLE;
        VkQueue presentQueue = VK_NULL_HANDLE;

        // Command pools
        VkCommandPool graphicsCommandPool = VK_NULL_HANDLE;

        // Extensions
        const std::array<const char *, 1> requiredExtensions{
            VK_KHR_SWAPCHAIN_EXTENSION_NAME,
        };

    private:
        VkPhysicalDevice getBestPhysicalDevice() const;
        VkDevice createDevice() const;
        VkCommandPool createGraphicsCommandPool() const;

        /**
         * @return uint32_t `0` means that the physical device is not suitable.
         */
        uint32_t ratePhysicalDevice(VkPhysicalDevice physicalDevice) const noexcept;
        uint32_t rateQueueFamilies(VkPhysicalDevice physicalDevice) const;
        uint32_t rateExtensions(VkPhysicalDevice physicalDevice) const;
        uint32_t rateProperties(VkPhysicalDevice physicalDevice) const noexcept;
        std::vector<VkExtensionProperties> getAvailableExtensions(VkPhysicalDevice physicalDevice) const;
        std::vector<const char *> getExtensionsToUse(VkPhysicalDevice physicalDevice) const;

        std::vector<VkQueueFamilyProperties> getQueueFamilies(VkPhysicalDevice physicalDevice) const;
        std::optional<uint32_t> getGraphicsQueueFamilyIndex(VkPhysicalDevice physicalDevice) const;
        std::optional<uint32_t> getPresentQueueFamilyIndex(VkPhysicalDevice physicalDevice) const;
};