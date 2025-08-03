#pragma once

#include "graphics/engines/Engine.hpp"
#include <vector>
#include <vulkan/vulkan_core.h>

class VulkanEngine : public Engine {
    public:
        VulkanEngine();
        ~VulkanEngine() override;
        void draw() const override;

    private:
        VkInstance instance = VK_NULL_HANDLE;

        VkInstance createInstance() const;
        std::vector<const char *> getRequiredExtensions() const noexcept;
};