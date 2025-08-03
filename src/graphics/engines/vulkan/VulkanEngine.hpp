#pragma once

#include "graphics/engines/Engine.hpp"

class VulkanEngine : public Engine {
    public:
        VulkanEngine();
        ~VulkanEngine() override;
        void draw() const override;
};