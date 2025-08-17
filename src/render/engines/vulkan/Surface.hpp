#pragma once

#include "Instance.hpp"
#include "render/targets/Window.hpp"
#include <vulkan/vulkan_core.h>

class Surface {
    public:
        Surface(const Instance &instance, const Window &window);
        ~Surface();
        VkSurfaceKHR getVkSurface() const noexcept;

    private:
        const Instance &instance;
        const Window &window;
        VkSurfaceKHR surface = VK_NULL_HANDLE;
};