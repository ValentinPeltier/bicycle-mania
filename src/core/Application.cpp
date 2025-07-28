#include "Application.hpp"
#include "Logger.hpp"
#include <memory>

Application::Application()
    : rootEntity(std::make_shared<Entity>("root")) {
    LOG_INFO("Application starting.");
}

Application::~Application() {
    LOG_INFO("Application exiting.");
}

void Application::run() {
    LOG_INFO("Application running.");
}