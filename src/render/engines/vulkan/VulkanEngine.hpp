#pragma once

#include "../RenderEngine.hpp"
#include "Device.hpp"
#include "Instance.hpp"
#include "Surface.hpp"
#include <GLFW/glfw3.h>

class VulkanEngine : public RenderEngine {
    public:
        VulkanEngine(GLFWwindow *window);
        ~VulkanEngine() override;

    private:
        GLFWwindow *window = nullptr;

        Instance instance;
        Device device;
        Surface surface;
};