#include "VulkanEngine.hpp"
#include "ImageViews.hpp"

VulkanEngine::VulkanEngine(const Window &window)
    : instance(),
      surface(this->instance, window),
      device(this->instance, this->surface),
      swapchain(window, this->surface, this->device),
      imageViews(this->device, this->swapchain) {
}

VulkanEngine::~VulkanEngine() {
}
