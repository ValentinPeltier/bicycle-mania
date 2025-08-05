#include "Application.hpp"
#include "core/Logger.hpp"
#include <exception>

int main() {
    try {
        Application application;
        application.run();
    } catch (const std::exception &e) {
        LOG_ERROR(e.what());
        return 1;
    }

    return 0;
}