#pragma once

#include <GLFW/glfw3.h>
#include <string>

class Window {
    public:
        Window(const std::string title);
        ~Window();
        GLFWwindow *getGLFWWindow() const noexcept;

        void setTitle(const std::string title) const;
        void setDefaultSize(uint32_t defaultWidth, uint32_t defaultHeight);
        void maximize() const;
        void unMaximize() const;

        bool shouldClose() const;
        void update() const;

    private:
        GLFWwindow *window = nullptr;
        uint32_t defaultWidth = 800;
        uint32_t defaultHeight = 600;
};