#include "Application.hpp"
#include "Logger.hpp"
#include "graphics/displayers/Window.hpp"
#include <memory>

Application::Application()
    : window(std::make_unique<Window>("My Windowww")),
      rootEntity(std::make_shared<Entity>("root")) {
    LOG_DEBUG("Application starting.");
}

Application::~Application() {
    LOG_DEBUG("Application exiting.");
}

void Application::run() {
    LOG_DEBUG("Application running.");
    while (!this->window->shouldClose()) {
        this->window->update();
    }
}