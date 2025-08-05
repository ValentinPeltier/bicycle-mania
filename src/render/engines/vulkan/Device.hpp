#pragma once

#include "Instance.hpp"
#include "render/engines/vulkan/Surface.hpp"
#include <cstdint>
#include <optional>
#include <vulkan/vulkan_core.h>

struct QueueFamilyIndices {
        std::optional<uint32_t> graphics;
        std::optional<uint32_t> present;
};

class Device {
    public:
        Device(const Instance &instance, const Surface &surface);
        ~Device();

    private:
        const Instance &instance;
        const Surface &surface;
        VkDevice device = VK_NULL_HANDLE;
        VkQueue graphicsQueue = VK_NULL_HANDLE;
        VkQueue presentQueue = VK_NULL_HANDLE;

        const std::vector<const char *> requiredExtensions{
            VK_KHR_SWAPCHAIN_EXTENSION_NAME,
        };

        std::vector<VkPhysicalDevice> getPhysicalDevices() const;
        VkPhysicalDevice getBestPhysicalDevice() const;
        /**
         * @return uint32_t `0` means that the physical device is not suitable for the engine.
         */
        uint32_t ratePhysicalDevice(VkPhysicalDevice physicalDevice) const noexcept;
        uint32_t ratePhysicalDeviceQueueFamilies(VkPhysicalDevice physicalDevice) const noexcept;
        uint32_t ratePhysicalDeviceExtensions(VkPhysicalDevice physicalDevice) const noexcept;
        uint32_t ratePhysicalDeviceProperties(VkPhysicalDevice physicalDevice) const noexcept;
        std::vector<VkExtensionProperties> getAvailableExtensions(VkPhysicalDevice physicalDevice) const;
        std::vector<const char *> getExtensionsToUse(VkPhysicalDevice physicalDevice) const;

        std::vector<VkDeviceQueueCreateInfo> getPhysicalDeviceQueueCreateInfos(VkPhysicalDevice physicalDevice) const;
        QueueFamilyIndices getPhysicalDeviceQueueFamilies(VkPhysicalDevice device) const noexcept;
};