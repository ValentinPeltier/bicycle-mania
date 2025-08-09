#pragma once

#include "render/engines/vulkan/Device.hpp"
#include <vulkan/vulkan_core.h>

class CommandPool {
    public:
        CommandPool(const Device &device);
        ~CommandPool();

        VkCommandPool getVkCommandPool() const noexcept;

    private:
        const Device &device;
        VkCommandPool commandPool = VK_NULL_HANDLE;
};