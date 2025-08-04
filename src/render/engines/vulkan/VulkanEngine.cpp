#include "VulkanEngine.hpp"
#include <GLFW/glfw3.h>

VulkanEngine::VulkanEngine(GLFWwindow *window)
    : window(window),
      instance(),
      device(this->instance),
      surface(this->instance, this->window) {
}

VulkanEngine::~VulkanEngine() {
}
