#include "VulkanEngine.hpp"

VulkanEngine::VulkanEngine()
    : instance(),
      device(this->instance) {
}

VulkanEngine::~VulkanEngine() {
}
