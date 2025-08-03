#pragma once

#include "Instance.hpp"
#include <vulkan/vulkan_core.h>

class Device {
    public:
        Device(const Instance &instance);
        ~Device();

    private:
        const Instance &instance;
        VkDevice device = VK_NULL_HANDLE;

        /**
         * @return uint32_t `0` means that the physical device is not suitable for the engine.
         */
        uint32_t ratePhysicalDevice(VkPhysicalDeviceProperties properties) const noexcept;
        VkPhysicalDevice getBestPhysicalDevice() const;
};