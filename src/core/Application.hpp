#pragma once

#include "game/Entity.hpp"
#include "render/engines/RenderEngine.hpp"
#include <memory>

class Application {
    private:
        std::unique_ptr<RenderEngine> renderEngine;
        std::shared_ptr<Entity> rootEntity;

    public:
        Application();
        ~Application();
        void run();
};