#pragma once

class Engine {
    public:
        virtual ~Engine() {};
        virtual void draw() const = 0;
};