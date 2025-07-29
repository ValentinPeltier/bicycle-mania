#include "Application.hpp"
#include "Logger.hpp"
#include <memory>

Application::Application()
    : rootEntity(std::make_shared<Entity>("root")) {
    LOG_DEBUG("Application starting.");
}

Application::~Application() {
    LOG_DEBUG("Application exiting.");
}

void Application::run() {
    LOG_DEBUG("Application running.");
}