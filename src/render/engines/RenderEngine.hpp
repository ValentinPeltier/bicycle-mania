#pragma once

#include "render/targets/Window.hpp"
#include <memory>

class RenderEngine {
    public:
        virtual ~RenderEngine() {};
        static std::unique_ptr<RenderEngine> getBest(const Window &window);

        virtual void draw() = 0;
};