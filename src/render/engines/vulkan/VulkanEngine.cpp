#include "VulkanEngine.hpp"
#include "core/Logger.hpp"

VulkanEngine::VulkanEngine()
    : instance(),
      device(this->instance) {
    LOG_DEBUG("Vulkan engine set up!");
}

VulkanEngine::~VulkanEngine() {
    LOG_DEBUG("Vulkan engine cleaned up.");
}
