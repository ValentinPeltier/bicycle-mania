#pragma once

#include <GLFW/glfw3.h>
#include <memory>

class RenderEngine {
    public:
        virtual ~RenderEngine() {};
        static std::unique_ptr<RenderEngine> getBest(GLFWwindow *window);
};