#pragma once

#include "game/Entity.hpp"
#include "graphics/engines/Engine.hpp"
#include <memory>

class Application {
    private:
        std::unique_ptr<Engine> engine;
        std::shared_ptr<Entity> rootEntity;

    public:
        Application();
        ~Application();
        void run();
};