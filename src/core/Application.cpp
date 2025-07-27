#include "Application.hpp"
#include <memory>

Application::Application()
    : rootEntity(std::make_shared<Entity>("root")) {
}

void Application::run() {
}