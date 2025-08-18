#include "Instance.hpp"
#include "core/Logger.hpp"
#include <GLFW/glfw3.h>
#include <stdexcept>
#include <vulkan/vulkan_core.h>

Instance::Instance() {
    // Create a VkInstance
    VkApplicationInfo applicationInfo{};
    applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    applicationInfo.pApplicationName = "GameTest";
    applicationInfo.applicationVersion = VK_MAKE_VERSION(0, 1, 0);
    applicationInfo.pEngineName = "No Engine";
    applicationInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    applicationInfo.apiVersion = VK_API_VERSION_1_3;

    auto extensions = this->getExtensionsToUse();
    auto instanceLayers = this->getLayersToUse();

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &applicationInfo;
    createInfo.enabledExtensionCount = extensions.size();
    createInfo.ppEnabledExtensionNames = extensions.data();
    createInfo.enabledLayerCount = instanceLayers.size();
    createInfo.ppEnabledLayerNames = instanceLayers.data();

#ifdef ENABLE_VALIDATION_LAYERS
    // Add validation layer for the instance creation itself
    VkDebugUtilsMessengerCreateInfoEXT validationLayerMessengerCreateInfo =
        this->getValidationLayerMessengerCreateInfo();
    createInfo.pNext = &validationLayerMessengerCreateInfo;
#endif

    if (vkCreateInstance(&createInfo, nullptr, &this->instance) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create Vulkan instance.");
    }

#ifdef ENABLE_VALIDATION_LAYERS
    this->validationLayerMessenger = this->createValidationLayerMessenger();
#endif
}

Instance::~Instance() {
    vkDestroyDebugUtilsMessengerEXT(this->instance, this->validationLayerMessenger, nullptr);
    vkDestroyInstance(this->instance, nullptr);
}

VkInstance Instance::getVkInstance() const noexcept {
    return this->instance;
}

// ---------------- //
// --- Instance --- //
// ---------------- //

std::vector<VkExtensionProperties> Instance::getAvailableExtensions() const {
    uint32_t availableExtensionCount;
    if (vkEnumerateInstanceExtensionProperties(nullptr, &availableExtensionCount, nullptr) != VK_SUCCESS) {
        throw std::runtime_error("Unable to get the available extensions.");
    }
    std::vector<VkExtensionProperties> availableExtensions(availableExtensionCount);
    if (vkEnumerateInstanceExtensionProperties(nullptr, &availableExtensionCount, availableExtensions.data()) !=
        VK_SUCCESS) {
        throw std::runtime_error("Unable to get the available extensions.");
    }

    return availableExtensions;
}

std::vector<const char *> Instance::getExtensionsToUse() const {
    // Start with the required extensions
    std::vector<const char *> extensions = this->requiredExtensions;

    // Add GLFW required extensions
    uint32_t glfwExtensionCount;
    auto glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    for (int i = 0; i < glfwExtensionCount; i++) {
        extensions.push_back(glfwExtensions[i]);
    }

    // Add optional extensions (none for now)

    return extensions;
}

std::vector<VkLayerProperties> Instance::getAvailableLayers() const {
    uint32_t availableLayerCount;
    if (vkEnumerateInstanceLayerProperties(&availableLayerCount, nullptr) != VK_SUCCESS) {
        throw std::runtime_error("Unable to get the available layers.");
    }
    std::vector<VkLayerProperties> availableLayers(availableLayerCount);
    if (vkEnumerateInstanceLayerProperties(&availableLayerCount, availableLayers.data()) != VK_SUCCESS) {
        throw std::runtime_error("Unable to get the available layers.");
    }

    return availableLayers;
}

std::vector<const char *> Instance::getLayersToUse() const {
    // Start with the required layers
    std::vector<const char *> layers = this->requiredLayers;

    // Add optional layers (none for now)

    return layers;
}

// ------------------------- //
// --- Validation layers --- //
// ------------------------- //

VkDebugUtilsMessengerEXT Instance::createValidationLayerMessenger() const {
    VkDebugUtilsMessengerCreateInfoEXT createInfo = this->getValidationLayerMessengerCreateInfo();

    VkDebugUtilsMessengerEXT validationLayerMessenger = VK_NULL_HANDLE;
    if (vkCreateDebugUtilsMessengerEXT(this->instance, &createInfo, nullptr, &validationLayerMessenger) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create validation layer messenger.");
    }

    return validationLayerMessenger;
}

VkDebugUtilsMessengerCreateInfoEXT Instance::getValidationLayerMessengerCreateInfo() const noexcept {
    VkDebugUtilsMessengerCreateInfoEXT createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity =
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                             VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                             VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = this->validationLayerMessageCallback;

    return createInfo;
}

VKAPI_ATTR VkBool32 VKAPI_CALL Instance::validationLayerMessageCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, void *pUserData) noexcept {

    if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
        LOG_ERROR(std::string("Validation layer: ") + pCallbackData->pMessage);
    } else if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
        LOG_WARNING(std::string("Validation layer: ") + pCallbackData->pMessage);
    }

    return VK_FALSE;
}
