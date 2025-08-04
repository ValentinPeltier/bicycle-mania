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

        VkPhysicalDevice getBestPhysicalDevice() const;
        /**
         * @return uint32_t `0` means that the physical device is not suitable for the engine.
         */
        uint32_t ratePhysicalDevice(VkPhysicalDevice device) const noexcept;

        std::vector<VkDeviceQueueCreateInfo> getQueueCreateInfo(VkPhysicalDevice physicalDevice) const;
        QueueFamilyIndices getQueueFamilies(VkPhysicalDevice device) const noexcept;
};