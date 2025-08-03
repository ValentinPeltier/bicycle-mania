#pragma once

#include "../RenderEngine.hpp"
#include "Device.hpp"
#include "Instance.hpp"

class VulkanEngine : public RenderEngine {
    public:
        VulkanEngine();
        ~VulkanEngine() override;

    private:
        Instance instance;
        Device device;
};