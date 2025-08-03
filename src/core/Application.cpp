#include "Application.hpp"
#include "Logger.hpp"
#include "graphics/engines/vulkan/VulkanEngine.hpp"
#include <GLFW/glfw3.h>
#include <memory>
#include <string>
#include <unistd.h>

Application::Application()
    : engine(std::make_unique<VulkanEngine>()),
      rootEntity(std::make_shared<Entity>("root")) {
    LOG_DEBUG("Application starting.");
    glfwInit();
}

Application::~Application() {
    LOG_DEBUG("Application exiting.");
    glfwTerminate();
}

void Application::run() {
    LOG_DEBUG("Application running.");

    while (true) {
        glfwPollEvents();
    }
}