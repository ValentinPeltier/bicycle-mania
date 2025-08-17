#include "Window.hpp"
#include "GLFWInitializer.hpp"
#include <GLFW/glfw3.h>
#include <cstdint>
#include <string>

Window::Window(const std::string title) {
    GLFWManager::initialize();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    glfwWindowHint(GLFW_FOCUSED, GLFW_TRUE);
    glfwWindowHint(GLFW_FOCUS_ON_SHOW, GLFW_TRUE);

    this->window = glfwCreateWindow(this->defaultWidth, this->defaultHeight, title.c_str(), nullptr, nullptr);
}

Window::~Window() {
    glfwDestroyWindow(this->window);

    GLFWManager::terminate();
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

WindowSize Window::getFramebufferSize() const {
    int width, height;
    glfwGetFramebufferSize(this->window, &width, &height);

    return {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};
}

void Window::update() const {
    glfwPollEvents();
}