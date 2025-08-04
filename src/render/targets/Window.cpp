#include "Window.hpp"
#include <GLFW/glfw3.h>
#include <cstdint>
#include <stdexcept>
#include <string>

uint32_t Window::windowCount = 0;

Window::Window(const std::string title) {
    if (Window::windowCount == 0) {
        if (glfwInit() == GLFW_FALSE) {
            throw std::runtime_error("Error while initializing GLFW.");
        }
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    glfwWindowHint(GLFW_FOCUSED, GLFW_TRUE);
    glfwWindowHint(GLFW_FOCUS_ON_SHOW, GLFW_TRUE);

    this->window = glfwCreateWindow(this->defaultWidth, this->defaultHeight, title.c_str(), nullptr, nullptr);
    ++Window::windowCount;
}

Window::~Window() {
    glfwDestroyWindow(this->window);

    // Once all windows are destroyed, terminate GLFW
    if (--Window::windowCount <= 0) {
        glfwTerminate();
    }
}

GLFWwindow *Window::getGLFWWindow() const noexcept {
    return this->window;
}

void Window::setTitle(const std::string title) const {
    glfwSetWindowTitle(this->window, title.c_str());
}

void Window::setDefaultSize(uint32_t defaultWidth, uint32_t defaultHeight) {
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