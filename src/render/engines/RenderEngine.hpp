#pragma once

#include <memory>

class RenderEngine {
    public:
        virtual ~RenderEngine() {};
        static std::unique_ptr<RenderEngine> getBest();
};