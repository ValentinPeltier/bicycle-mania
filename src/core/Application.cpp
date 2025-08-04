#include "Application.hpp"
#include "game/Entity.hpp"
#include "render/engines/RenderEngine.hpp"
#include "render/targets/Window.hpp"
#include <GLFW/glfw3.h>
#include <csignal>
#include <memory>
#include <unistd.h>

bool Application::sigint = false;

Application::Application()
    : window(std::make_unique<Window>("Bicycle Mania")),
      renderEngine(std::move(RenderEngine::getBest(this->window.get()->getGLFWWindow()))),
      rootEntity(std::make_shared<Entity>("root")) {
    // Signal handlers
    std::signal(SIGINT, Application::sigintCallback);
}

Application::~Application() {
}

void Application::sigintCallback(int signal) {
    Application::sigint = true;
}

void Application::run() {
    while (!Application::sigint && !this->window->shouldClose()) {
        this->window->update();
    }
}