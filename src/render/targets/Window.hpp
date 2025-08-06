#pragma once

#include <GLFW/glfw3.h>
#include <cstdint>
#include <string>

struct WindowSize {
        uint32_t width;
        uint32_t height;
};

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
        WindowSize getFramebufferSize() const;
        void update() const;

    private:
        GLFWwindow *window = nullptr;
        uint32_t defaultWidth = 800;
        uint32_t defaultHeight = 600;
};