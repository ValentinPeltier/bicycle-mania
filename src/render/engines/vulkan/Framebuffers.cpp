#include "Framebuffers.hpp"
#include "render/engines/vulkan/Device.hpp"
#include "render/engines/vulkan/RenderPass.hpp"
#include <cstdint>
#include <stdexcept>
#include <vulkan/vulkan_core.h>

Framebuffers::Framebuffers(
    const Device &device, const Swapchain &swapchain, const ImageViews &imageViews, const RenderPass &renderPass)
    : device(device),
      imageViews(imageViews),
      swapchain(swapchain),
      renderPass(renderPass) {
    this->framebuffers.resize(imageViews.getSize());

    for (uint32_t i = 0; i < this->framebuffers.size(); i++) {
        std::vector<VkImageView> attachments{
            imageViews.getVkImageViews()[i],
        };

        VkFramebufferCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        createInfo.renderPass = this->renderPass.getVkRenderPass();
        createInfo.attachmentCount = attachments.size();
        createInfo.pAttachments = attachments.data();
        createInfo.width = swapchain.getExtent().width;
        createInfo.height = swapchain.getExtent().height;
        createInfo.layers = 1;

        if (vkCreateFramebuffer(this->device.getVkDevice(), &createInfo, nullptr, &this->framebuffers[i]) !=
            VK_SUCCESS) {
            throw std::runtime_error("Failed to create the framebuffers.");
        }
    }
}

Framebuffers::~Framebuffers() {
    for (auto &framebuffer : this->framebuffers) {
        vkDestroyFramebuffer(this->device.getVkDevice(), framebuffer, nullptr);
    }
}