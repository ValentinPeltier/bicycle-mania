#include "Window.hpp"
#include "core/Logger.hpp"
#include <GLFW/glfw3.h>
#include <stdexcept>

int Window::windowCount = 0;

Window::Window(const char *title) {
    if (Window::windowCount == 0) {
        if (glfwInit() == GLFW_FALSE) {
            throw std::runtime_error("Error while initializing GLFW.");
        }

        LOG_DEBUG("GLFW set up.");
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    glfwWindowHint(GLFW_FOCUSED, GLFW_TRUE);
    glfwWindowHint(GLFW_FOCUS_ON_SHOW, GLFW_TRUE);

    this->window = glfwCreateWindow(this->defaultWidth, this->defaultHeight, title, nullptr, nullptr);
    ++Window::windowCount;
    LOG_DEBUG("Window created.");
}

Window::~Window() {
    glfwDestroyWindow(this->window);
    LOG_DEBUG("Window destroyed.");

    // Once all windows are destroyed, terminate GLFW
    if (--Window::windowCount <= 0) {
        glfwTerminate();
        LOG_DEBUG("GLFW cleaned up.");
    }
}

void Window::setTitle(const char *title) const {
    glfwSetWindowTitle(this->window, title);
}

void Window::setDefaultSize(int defaultWidth, int defaultHeight) {
    this->defaultWidth = defaultWidth;
    this->defaultHeight = defaultHeight;
}

void Window::maximize() const {
    glfwMaximizeWindow(this->window);
}

void Window::unMaximize() const {
    glfwRestoreWindow(this->window);
}

bool Window::shouldClose() const {
    return glfwWindowShouldClose(this->window);
}

void Window::update() const {
    glfwPollEvents();
}