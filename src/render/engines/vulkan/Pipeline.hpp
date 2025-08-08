#pragma once

#include "Device.hpp"
#include "RenderPass.hpp"
#include "Swapchain.hpp"
#include <vector>
#include <vulkan/vulkan_core.h>

class Pipeline {
    public:
        Pipeline(const Device &device, const Swapchain &swapchain, const RenderPass &renderPass);
        ~Pipeline();

    private:
        const Device &device;
        const Swapchain &swapchain;
        const RenderPass &renderPass;
        VkPipelineLayout layout = VK_NULL_HANDLE;
        VkPipeline pipeline = VK_NULL_HANDLE;

        VkShaderModule createShaderModule(const std::vector<char> &code) const;
        void destroyShaderModule(VkShaderModule shaderModule) const;
};