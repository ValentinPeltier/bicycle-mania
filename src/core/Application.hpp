#pragma once

#include "game/Entity.hpp"
#include "graphics/displayers/Window.hpp"
#include <memory>

class Application {
    private:
        std::unique_ptr<Window> window;
        std::shared_ptr<Entity> rootEntity;

    public:
        Application();
        ~Application();
        void run();
};