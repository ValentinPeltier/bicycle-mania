#pragma once

#include "Instance.hpp"
#include <GLFW/glfw3.h>
#include <vulkan/vulkan_core.h>

class Surface {
    public:
        Surface(const Instance &instance, GLFWwindow *window);
        ~Surface();

    private:
        const Instance &instance;
        GLFWwindow *window = nullptr;
        VkSurfaceKHR surface = VK_NULL_HANDLE;
};