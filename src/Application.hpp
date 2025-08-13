#pragma once

#include "game/Entity.hpp"
#include "render/engines/RenderEngine.hpp"
#include "render/targets/Window.hpp"
#include <csignal>
#include <memory>

class Application {
    public:
        Application()
            : window(std::make_unique<Window>("Bicycle Mania")),
              renderEngine(std::move(RenderEngine::getBest(*this->window))),
              rootEntity(std::make_shared<Entity>("root")) {
            // Handle SIGINT signal
            std::signal(SIGINT, Application::sigintCallback);
        }

        void run() {
            while (!Application::sigint && !this->window->shouldClose()) {
                this->window->update();
                this->renderEngine->draw();
            }
        }

        static void sigintCallback(int signal) {
            Application::sigint = true;
        }

    private:
        std::unique_ptr<Window> window;
        std::unique_ptr<RenderEngine> renderEngine;
        std::shared_ptr<Entity> rootEntity;

        inline static bool sigint = false;
};