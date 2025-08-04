#pragma once

#include "Instance.hpp"
#include <cstdint>
#include <optional>
#include <vulkan/vulkan_core.h>

struct QueueFamilyIndices {
        std::optional<uint32_t> graphicsFamily;
};

class Device {
    public:
        Device(const Instance &instance);
        ~Device();

    private:
        const Instance &instance;
        VkDevice device = VK_NULL_HANDLE;
        VkQueue graphicsQueue = VK_NULL_HANDLE;

        VkPhysicalDevice getBestPhysicalDevice() const;
        /**
         * @return uint32_t `0` means that the physical device is not suitable for the engine.
         */
        static uint32_t ratePhysicalDevice(VkPhysicalDevice device) noexcept;

        std::vector<VkDeviceQueueCreateInfo> getQueueCreateInfo(VkPhysicalDevice physicalDevice) const;
        static QueueFamilyIndices getQueueFamilies(VkPhysicalDevice device);
};