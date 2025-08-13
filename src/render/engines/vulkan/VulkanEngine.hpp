#pragma once

#include "../RenderEngine.hpp"
#include "CommandBuffers.hpp"
#include "CommandPool.hpp"
#include "Device.hpp"
#include "Instance.hpp"
#include "Pipeline.hpp"
#include "Surface.hpp"
#include "Swapchain.hpp"
#include "render/targets/Window.hpp"
#include <vulkan/vulkan_core.h>

class VulkanEngine : public RenderEngine {
    public:
        VulkanEngine(const Window &window);
        ~VulkanEngine() override;

        void draw() override;

    private:
        Instance instance;
        Surface surface;
        Device device;
        Swapchain swapchain;
        Pipeline pipeline;

        const int MAX_FRAMES_WAITING = 2;
        uint32_t frameIndex = 0;

        CommandPool commandPool;
        CommandBuffers commandBuffers;

        std::vector<VkSemaphore> imageAvailableSemaphores{};
        std::vector<VkSemaphore> renderFinishedSemaphores{};
        std::vector<VkFence> framePresentedFences{};
};