#pragma once

#include "game/Entity.hpp"
#include <memory>

class Application {
    private:
        std::shared_ptr<Entity> rootEntity;

    public:
        Application();
        ~Application();
        void run();
};