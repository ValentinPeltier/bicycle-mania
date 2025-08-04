#include "Surface.hpp"
#include "GLFW/glfw3.h"
#include "Instance.hpp"
#include <stdexcept>
#include <vulkan/vulkan_core.h>

Surface::Surface(const Instance &instance, GLFWwindow *window)
    : instance(instance),
      window(window) {
    if (glfwCreateWindowSurface(this->instance.getVkInstance(), this->window, nullptr, &this->surface) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create the window surface.");
    }
}

Surface::~Surface() {
    vkDestroySurfaceKHR(this->instance.getVkInstance(), this->surface, nullptr);
}