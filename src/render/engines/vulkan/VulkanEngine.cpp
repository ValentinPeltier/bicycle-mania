#include "VulkanEngine.hpp"

VulkanEngine::VulkanEngine(const Window &window)
    : instance(),
      device(this->instance),
      surface(this->instance, window) {
}

VulkanEngine::~VulkanEngine() {
}
