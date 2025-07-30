#pragma once

#include <GLFW/glfw3.h>

class Window {
    private:
        static int windowCount;
        GLFWwindow *window = nullptr;
        int defaultWidth = 800;
        int defaultHeight = 600;

    public:
        Window(const char *title);
        ~Window();

        void setTitle(const char *title) const;
        void setDefaultSize(int defaultWidth, int defaultHeight);
        void maximize() const;
        void unMaximize() const;

        bool shouldClose() const;
        void update() const;
};