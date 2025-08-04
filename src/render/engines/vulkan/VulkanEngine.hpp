#pragma once

#include "../RenderEngine.hpp"
#include "Device.hpp"
#include "Instance.hpp"
#include "Surface.hpp"
#include "render/targets/Window.hpp"

class VulkanEngine : public RenderEngine {
    public:
        VulkanEngine(const Window &window);
        ~VulkanEngine() override;

    private:
        Instance instance;
        Device device;
        Surface surface;
};