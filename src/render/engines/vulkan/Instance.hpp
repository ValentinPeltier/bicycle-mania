#pragma once

#include <vector>
#include <vulkan/vulkan_core.h>

#ifdef DEBUG
    #define ENABLE_VALIDATION_LAYERS
#endif

class Instance {
    public:
        Instance();
        ~Instance();
        const VkInstance &getVkInstance() const noexcept;

    private:
        VkInstance instance = VK_NULL_HANDLE;
        VkDebugUtilsMessengerEXT validationLayerMessenger = VK_NULL_HANDLE;

        const std::vector<const char *> requiredExtensions{
#ifdef ENABLE_VALIDATION_LAYERS
            VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
#endif
        };

        const std::vector<const char *> requiredLayers{
#ifdef ENABLE_VALIDATION_LAYERS
            "VK_LAYER_KHRONOS_validation",
#endif
        };

        std::vector<VkExtensionProperties> getAvailableExtensions() const;
        std::vector<const char *> getExtensionsToUse() const;

        std::vector<VkLayerProperties> getAvailableLayers() const;
        std::vector<const char *> getLayersToUse() const;

        VkDebugUtilsMessengerEXT createValidationLayerMessenger() const;
        VkDebugUtilsMessengerCreateInfoEXT getValidationLayerMessengerCreateInfo() const noexcept;
        static VKAPI_ATTR VkBool32 VKAPI_CALL validationLayerMessageCallback(
            VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType,
            const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, void *pUserData) noexcept;
};