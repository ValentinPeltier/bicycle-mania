#include "Application.hpp"
#include "game/Entity.hpp"
#include "render/engines/RenderEngine.hpp"
#include "render/targets/Window.hpp"
#include <csignal>
#include <memory>

Application::Application()
    : window(std::make_unique<Window>("Bicycle Mania")),
      renderEngine(std::move(RenderEngine::getBest(*this->window))),
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