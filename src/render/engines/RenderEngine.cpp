#include "RenderEngine.hpp"
#include "vulkan/VulkanEngine.hpp"
#include <memory>

std::unique_ptr<RenderEngine> RenderEngine::getBest(const Window &window) {
    // Choose the best graphics engine for the hardware
    return std::make_unique<VulkanEngine>(window);
}