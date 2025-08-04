#pragma once

#include "game/Entity.hpp"
#include "render/targets/Window.hpp"
#include <memory>

class Application {
    public:
        Application();
        ~Application();
        void run();

    private:
        std::unique_ptr<Window> window;
        std::shared_ptr<Entity> rootEntity;
};