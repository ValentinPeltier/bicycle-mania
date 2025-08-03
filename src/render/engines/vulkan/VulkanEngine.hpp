#pragma once

#include "render/engines/RenderEngine.hpp"
#include <vector>
#include <vulkan/vk_platform.h>
#include <vulkan/vulkan_core.h>

class VulkanEngine : public RenderEngine {
    public:
        VulkanEngine();
        ~VulkanEngine() override;

    private:
        VkInstance instance = VK_NULL_HANDLE;
        VkDebugUtilsMessengerEXT validationLayerMessenger = VK_NULL_HANDLE;
        VkDevice device = VK_NULL_HANDLE;

#ifdef DEBUG
        const bool enableValidationLayers = true;
#else
        const bool enableValidationLayers = false;
#endif

        VkInstance createInstance() const;
        std::vector<VkExtensionProperties> getAvailableExtensions() const;
        std::vector<const char *> getExtensions() const;
        std::vector<VkLayerProperties> getAvailableLayers() const;
        std::vector<const char *> getLayers() const;

        VkDebugUtilsMessengerEXT createValidationLayerMessenger() const;
        VkDebugUtilsMessengerCreateInfoEXT getValidationLayerMessengerCreateInfo() const noexcept;
        static VKAPI_ATTR VkBool32 VKAPI_CALL validationLayerMessageCallback(
            VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType,
            const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, void *pUserData) noexcept;

        VkDevice createDevice() const;
        /**
         * @return uint32_t `0` means that the physical device is not suitable for the engine.
         */
        uint32_t ratePhysicalDevice(VkPhysicalDeviceProperties properties) const noexcept;
        VkPhysicalDevice getPreferredPhysicalDevice() const;
};