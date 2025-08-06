#pragma once

#include "Instance.hpp"
#include "Surface.hpp"
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

        const VkDevice &getVkDevice() const noexcept;
        const VkPhysicalDevice &getVkPhysicalDevice() const noexcept;

        QueueFamilyIndices getQueueFamilyIndices() const;

    private:
        const Instance &instance;
        const Surface &surface;
        VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
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
        uint32_t rateQueueFamilies(VkPhysicalDevice physicalDevice) const;
        uint32_t rateExtensions(VkPhysicalDevice physicalDevice) const;
        uint32_t rateProperties(VkPhysicalDevice physicalDevice) const noexcept;
        std::vector<VkExtensionProperties> getAvailableExtensions(VkPhysicalDevice physicalDevice) const;
        std::vector<const char *> getExtensionsToUse(VkPhysicalDevice physicalDevice) const;

        std::vector<VkDeviceQueueCreateInfo> getQueueCreateInfos(VkPhysicalDevice physicalDevice) const;
        QueueFamilyIndices getQueueFamilyIndices(VkPhysicalDevice physicalDevice) const;
};