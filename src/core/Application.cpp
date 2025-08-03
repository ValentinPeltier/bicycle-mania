#include "Application.hpp"
#include "Logger.hpp"
#include "game/Entity.hpp"
#include "render/engines/vulkan/VulkanEngine.hpp"
#include <GLFW/glfw3.h>
#include <memory>
#include <string>
#include <unistd.h>

Application::Application() {
    LOG_DEBUG("Application starting.");

    glfwInit();

    this->renderEngine = std::make_unique<VulkanEngine>();
    this->rootEntity = std::make_shared<Entity>("root");
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