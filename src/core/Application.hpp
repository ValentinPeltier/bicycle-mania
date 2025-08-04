#pragma once

#include "game/Entity.hpp"
#include "render/engines/RenderEngine.hpp"
#include "render/targets/Window.hpp"
#include <memory>

class Application {
    public:
        Application();
        ~Application();
        void run();

        static void sigintCallback(int signal);

    private:
        std::unique_ptr<Window> window;
        std::unique_ptr<RenderEngine> renderEngine;
        std::shared_ptr<Entity> rootEntity;

        inline static bool sigint = false;
};