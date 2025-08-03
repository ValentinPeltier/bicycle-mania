#include "Application.hpp"
#include "game/Entity.hpp"
#include "render/engines/RenderEngine.hpp"
#include <GLFW/glfw3.h>
#include <csignal>
#include <memory>
#include <unistd.h>

bool Application::sigint = false;

Application::Application() {
    // Signal handlers
    std::signal(SIGINT, Application::sigintCallback);

    // GLFW
    glfwInit();

    this->renderEngine = std::move(RenderEngine::getBest());
    this->rootEntity = std::make_shared<Entity>("root");
}

Application::~Application() {
    // GLFW
    glfwTerminate();
}

void Application::sigintCallback(int signal) {
    Application::sigint = true;
}

void Application::run() {
    while (!Application::sigint) {
        glfwPollEvents();
    }
}