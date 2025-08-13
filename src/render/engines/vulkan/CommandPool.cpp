#include "CommandPool.hpp"
#include "Device.hpp"
#include <stdexcept>
#include <vulkan/vulkan_core.h>

CommandPool::CommandPool(const Device &device)
    : device(device) {

    VkCommandPoolCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    createInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    createInfo.queueFamilyIndex = this->device.getQueueFamilyIndices().graphics.value();

    if (vkCreateCommandPool(this->device.getVkDevice(), &createInfo, nullptr, &this->commandPool) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create command pool.");
    }
}

CommandPool::~CommandPool() {
    vkDestroyCommandPool(this->device.getVkDevice(), this->commandPool, nullptr);
}

VkCommandPool CommandPool::getVkCommandPool() const noexcept {
    return this->commandPool;
}