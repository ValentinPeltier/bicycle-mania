#pragma once

#include <GLFW/glfw3.h>
#include <cstdint>
#include <string>
#include <vector>

class Gamepad {
    public:
        static std::vector<Gamepad> getAll();

        std::string getGUID() const noexcept;
        std::string getName() const noexcept;
        uint32_t getAxesCount() const noexcept;
        uint32_t getButtonsCount() const noexcept;

        void updateValues() noexcept;

        float getAxisLeftXValue() const noexcept;
        float getAxisLeftYValue() const noexcept;
        float getAxisRightXValue() const noexcept;
        float getAxisRightYValue() const noexcept;
        float getAxisLeftTriggerValue() const noexcept;
        float getAxisRightTriggerValue() const noexcept;

        bool isButtonAPressed() const noexcept;
        bool isButtonBPressed() const noexcept;
        bool isButtonXPressed() const noexcept;
        bool isButtonYPressed() const noexcept;
        bool isButtonLeftBumperPressed() const noexcept;
        bool isButtonRightBumperPressed() const noexcept;
        bool isButtonBackPressed() const noexcept;
        bool isButtonStartPressed() const noexcept;
        bool isButtonGuidePressed() const noexcept;
        bool isButtonLeftThumbPressed() const noexcept;
        bool isButtonRightThumbPressed() const noexcept;
        bool isButtonDpadUpPressed() const noexcept;
        bool isButtonDpadRightPressed() const noexcept;
        bool isButtonDpadDownPressed() const noexcept;
        bool isButtonDpadLeftPressed() const noexcept;

        static void connectionCallback(int id, int event);

    private:
        static bool initialized;
        uint32_t id;
        uint32_t axesCount;
        uint32_t buttonsCount;

        // State
        const float *axesValues = nullptr;
        const unsigned char *buttonsValues = nullptr;

        // Default axes mapping
        int axisLeftX = GLFW_GAMEPAD_AXIS_LEFT_X;
        int axisLeftY = GLFW_GAMEPAD_AXIS_LEFT_Y;
        int axisRightX = GLFW_GAMEPAD_AXIS_RIGHT_X;
        int axisRightY = GLFW_GAMEPAD_AXIS_RIGHT_Y;
        int axisLeftTrigger = GLFW_GAMEPAD_AXIS_LEFT_TRIGGER;
        int axisRightTrigger = GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER;

        // Default buttons mapping
        int buttonA = GLFW_GAMEPAD_BUTTON_A;
        int buttonB = GLFW_GAMEPAD_BUTTON_B;
        int buttonX = GLFW_GAMEPAD_BUTTON_X;
        int buttonY = GLFW_GAMEPAD_BUTTON_Y;
        int buttonLeftBumper = GLFW_GAMEPAD_BUTTON_LEFT_BUMPER;
        int buttonRightBumper = GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER;
        int buttonBack = GLFW_GAMEPAD_BUTTON_BACK;
        int buttonStart = GLFW_GAMEPAD_BUTTON_START;
        int buttonGuide = GLFW_GAMEPAD_BUTTON_GUIDE;
        int buttonLeftThumb = GLFW_GAMEPAD_BUTTON_LEFT_THUMB;
        int buttonRightThumb = GLFW_GAMEPAD_BUTTON_RIGHT_THUMB;
        int buttonDpadUp = GLFW_GAMEPAD_BUTTON_DPAD_UP;
        int buttonDpadRight = GLFW_GAMEPAD_BUTTON_DPAD_RIGHT;
        int buttonDpadDown = GLFW_GAMEPAD_BUTTON_DPAD_DOWN;
        int buttonDpadLeft = GLFW_GAMEPAD_BUTTON_DPAD_LEFT;

        Gamepad(uint32_t id);
        static void initialize();
};