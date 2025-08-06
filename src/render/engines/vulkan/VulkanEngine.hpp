#pragma once

#include "../RenderEngine.hpp"
#include "Device.hpp"
#include "ImageViews.hpp"
#include "Instance.hpp"
#include "Surface.hpp"
#include "Swapchain.hpp"
#include "render/targets/Window.hpp"

class VulkanEngine : public RenderEngine {
    public:
        VulkanEngine(const Window &window);
        ~VulkanEngine() override;

    private:
        Instance instance;
        Surface surface;
        Device device;
        Swapchain swapchain;
        ImageViews imageViews;
};