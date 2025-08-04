#include "Application.hpp"
#include "render/targets/Window.hpp"
#include <memory>

Application::Application()
    : window(std::make_unique<Window>("Bicycle Mania")),
      rootEntity(std::make_shared<Entity>("root")) {
}

Application::~Application() {
}

void Application::run() {
    while (!this->window->shouldClose()) {
        this->window->update();
    }
}