#pragma once

#include <vector>
#include <vulkan/vulkan_core.h>

class Instance {
    public:
        Instance();
        ~Instance();
        const VkInstance &getVkInstance() const noexcept;

    private:
        VkInstance instance = VK_NULL_HANDLE;
        VkDebugUtilsMessengerEXT validationLayerMessenger = VK_NULL_HANDLE;

        std::vector<VkExtensionProperties> getAvailableExtensions() const;
        std::vector<const char *> getExtensions() const;

        std::vector<VkLayerProperties> getAvailableLayers() const;
        std::vector<const char *> getLayers() const;

        VkDebugUtilsMessengerEXT createValidationLayerMessenger() const;
        VkDebugUtilsMessengerCreateInfoEXT getValidationLayerMessengerCreateInfo() const noexcept;
        static VKAPI_ATTR VkBool32 VKAPI_CALL validationLayerMessageCallback(
            VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType,
            const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, void *pUserData) noexcept;
};