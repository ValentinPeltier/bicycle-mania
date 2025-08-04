#include "VulkanEngine.hpp"

VulkanEngine::VulkanEngine(const Window &window)
    : instance(),
      surface(this->instance, window),
      device(this->instance, this->surface) {
}

VulkanEngine::~VulkanEngine() {
}
