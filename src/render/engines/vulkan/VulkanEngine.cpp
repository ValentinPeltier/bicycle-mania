#include "VulkanEngine.hpp"

VulkanEngine::VulkanEngine(const Window &window)
    : instance(),
      surface(this->instance, window),
      device(this->instance, this->surface),
      swapchain(window, this->surface, this->device),
      imageViews(this->device, this->swapchain),
      renderPass(this->device, this->swapchain),
      pipeline(this->device, this->swapchain, this->renderPass) {
}

VulkanEngine::~VulkanEngine() {
}
