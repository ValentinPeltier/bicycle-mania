#include "Surface.hpp"
#include "GLFW/glfw3.h"
#include "Instance.hpp"
#include "render/targets/Window.hpp"
#include <stdexcept>
#include <vulkan/vulkan_core.h>

Surface::Surface(const Instance &instance, const Window &window)
    : instance(instance),
      window(window) {
    if (glfwCreateWindowSurface(
            this->instance.getVkInstance(), this->window.getGLFWWindow(), nullptr, &this->surface) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create the window surface.");
    }
}

Surface::~Surface() {
    vkDestroySurfaceKHR(this->instance.getVkInstance(), this->surface, nullptr);
}

VkSurfaceKHR Surface::getVkSurface() const noexcept {
    return this->surface;
}