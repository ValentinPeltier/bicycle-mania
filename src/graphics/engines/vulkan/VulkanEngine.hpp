#pragma once

#include "graphics/engines/Engine.hpp"
#include <vector>
#include <vulkan/vk_platform.h>
#include <vulkan/vulkan_core.h>

class VulkanEngine : public Engine {
    public:
        VulkanEngine();
        ~VulkanEngine() override;
        void draw() const override;

    private:
        VkInstance instance = VK_NULL_HANDLE;
        VkDebugUtilsMessengerEXT validationLayerMessenger = VK_NULL_HANDLE;

        std::vector<const char *> validationLayers = {
            "VK_LAYER_KHRONOS_validation",
        };
#ifdef DEBUG
        const bool enableValidationLayers = true;
#else
        const bool enableValidationLayers = false;
#endif

        VkInstance createInstance() const;
        std::vector<const char *> getRequiredExtensions() const noexcept;

        bool hasValidationLayersSupport() const noexcept;
        VkDebugUtilsMessengerEXT createValidationLayerMessenger() const;
        VkDebugUtilsMessengerCreateInfoEXT getValidationLayerMessengerCreateInfo() const noexcept;
        static VKAPI_ATTR VkBool32 VKAPI_CALL validationLayerMessageCallback(
            VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType,
            const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, void *pUserData) noexcept;
};