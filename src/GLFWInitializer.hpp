#pragma once

#include <GLFW/glfw3.h>
#include <cstdint>
#include <stdexcept>

/**
 * @brief A wrapper around `glfwInit` and `glfwTerminate` to call them when necessary.
 */
class GLFWManager {
    public:
        static void initialize() {
            if (GLFWManager::initializationCount++ == 0) {
                if (glfwInit() != GLFW_TRUE) {
                    throw std::runtime_error("Failed to initialize GLFW.");
                }
            }
        }

        static void terminate() noexcept {
            if (--GLFWManager::initializationCount) {
                glfwTerminate();
            }
        }

    private:
        inline static uint32_t initializationCount = 0;

        GLFWManager();
};
