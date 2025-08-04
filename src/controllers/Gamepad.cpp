#include "Gamepad.hpp"
#include "GLFWInitializer.hpp"
#include "core/File.hpp"
#include "core/Logger.hpp"
#include <GLFW/glfw3.h>
#include <string>

Gamepad::Gamepad(uint32_t id)
    : id(id) {
    GLFWManager::initialize();

    if (!Gamepad::initialized) {
        Gamepad::initialize();
    }

    this->updateValues();
}

Gamepad::~Gamepad() {
    GLFWManager::terminate();
}

std::vector<Gamepad> Gamepad::getAll() {
    std::vector<Gamepad> gamepads{};

    // GLFW support up to 16 controllers
    for (int id = 0; id < 16; id++) {
        if (glfwJoystickPresent(id)) {
            gamepads.push_back(Gamepad(id));
        }
    }

    return gamepads;
}

std::string Gamepad::getGUID() const noexcept {
    return std::string(glfwGetJoystickGUID(this->id));
}

std::string Gamepad::getName() const noexcept {
    return std::string(glfwGetJoystickName(this->id));
}

uint32_t Gamepad::getAxesCount() const noexcept {

    return this->axesCount;
}

uint32_t Gamepad::getButtonsCount() const noexcept {
    return this->buttonsCount;
}

void Gamepad::updateValues() noexcept {
    int axesCount;
    this->axesValues = glfwGetJoystickAxes(this->id, &axesCount);
    if (axesCount == 0) {
        LOG_ERROR("An error occured while retrieving the number of axes of the gamepad.");
    }
    this->axesCount = static_cast<uint32_t>(axesCount);

    int buttonsCount;
    this->buttonsValues = glfwGetJoystickButtons(this->id, &buttonsCount);
    if (buttonsCount == 0) {
        LOG_ERROR("An error occured while retrieving the number of buttons of the gamepad.");
    }
    this->buttonsCount = static_cast<uint32_t>(buttonsCount);
}

void Gamepad::connectionCallback(int id, int event) {
    if (event == GLFW_CONNECTED) {
        LOG_DEBUG("Gamepad " + std::to_string(id) + " connected.");
    } else if (event == GLFW_DISCONNECTED) {
        LOG_DEBUG("Gamepad " + std::to_string(id) + " disconnected.");
    }
}

void Gamepad::initialize() {
    if (glfwUpdateGamepadMappings(File(Path("share/gamecontrollerdb.txt")).read().c_str()) != GLFW_TRUE) {
        LOG_ERROR("Failed to update gamepad mappings.");
    }

    glfwSetJoystickCallback(Gamepad::connectionCallback);

    Gamepad::initialized = true;
}

float Gamepad::getAxisLeftXValue() const noexcept {
    return this->axesValues[this->axisLeftX];
}

float Gamepad::getAxisLeftYValue() const noexcept {
    return this->axesValues[this->axisLeftY];
}

float Gamepad::getAxisRightXValue() const noexcept {
    return this->axesValues[this->axisRightX];
}

float Gamepad::getAxisRightYValue() const noexcept {
    return this->axesValues[this->axisRightY];
}

float Gamepad::getAxisLeftTriggerValue() const noexcept {
    return this->axesValues[this->axisLeftTrigger];
}

float Gamepad::getAxisRightTriggerValue() const noexcept {
    return this->axesValues[this->axisRightTrigger];
}

bool Gamepad::isButtonAPressed() const noexcept {
    return this->buttonsValues[this->buttonA] == GLFW_PRESS;
}

bool Gamepad::isButtonBPressed() const noexcept {
    return this->buttonsValues[this->buttonB] == GLFW_PRESS;
}

bool Gamepad::isButtonXPressed() const noexcept {
    return this->buttonsValues[this->buttonX] == GLFW_PRESS;
}

bool Gamepad::isButtonYPressed() const noexcept {
    return this->buttonsValues[this->buttonY] == GLFW_PRESS;
}

bool Gamepad::isButtonLeftBumperPressed() const noexcept {
    return this->buttonsValues[this->buttonLeftBumper] == GLFW_PRESS;
}

bool Gamepad::isButtonRightBumperPressed() const noexcept {
    return this->buttonsValues[this->buttonRightBumper] == GLFW_PRESS;
}

bool Gamepad::isButtonBackPressed() const noexcept {
    return this->buttonsValues[this->buttonBack] == GLFW_PRESS;
}

bool Gamepad::isButtonStartPressed() const noexcept {
    return this->buttonsValues[this->buttonStart] == GLFW_PRESS;
}

bool Gamepad::isButtonGuidePressed() const noexcept {
    return this->buttonsValues[this->buttonGuide] == GLFW_PRESS;
}

bool Gamepad::isButtonLeftThumbPressed() const noexcept {
    return this->buttonsValues[this->buttonLeftThumb] == GLFW_PRESS;
}

bool Gamepad::isButtonRightThumbPressed() const noexcept {
    return this->buttonsValues[this->buttonRightThumb] == GLFW_PRESS;
}

bool Gamepad::isButtonDpadUpPressed() const noexcept {
    return this->buttonsValues[this->buttonDpadUp] == GLFW_PRESS;
}

bool Gamepad::isButtonDpadRightPressed() const noexcept {
    return this->buttonsValues[this->buttonDpadRight] == GLFW_PRESS;
}

bool Gamepad::isButtonDpadDownPressed() const noexcept {
    return this->buttonsValues[this->buttonDpadDown] == GLFW_PRESS;
}

bool Gamepad::isButtonDpadLeftPressed() const noexcept {
    return this->buttonsValues[this->buttonDpadLeft] == GLFW_PRESS;
}
