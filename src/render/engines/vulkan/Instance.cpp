#include "Instance.hpp"
#include "core/Logger.hpp"
#include <GLFW/glfw3.h>
#include <stdexcept>

#ifdef DEBUG
    #define ENABLE_VALIDATION_LAYERS
#endif

Instance::Instance() {
    // Create a VkInstance
    VkApplicationInfo applicationInfo{};
    applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    applicationInfo.pApplicationName = "GameTest";
    applicationInfo.applicationVersion = VK_MAKE_VERSION(0, 1, 0);
    applicationInfo.pEngineName = "No Engine";
    applicationInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    applicationInfo.apiVersion = VK_API_VERSION_1_4;

    auto extensions = this->getExtensions();
    auto instanceLayers = this->getLayers();

    VkInstanceCreateInfo instanceCreateInfo{};
    instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceCreateInfo.pApplicationInfo = &applicationInfo;
    instanceCreateInfo.enabledExtensionCount = extensions.size();
    instanceCreateInfo.ppEnabledExtensionNames = extensions.data();
    instanceCreateInfo.enabledLayerCount = instanceLayers.size();
    instanceCreateInfo.ppEnabledLayerNames = instanceLayers.data();

#ifdef ENABLE_VALIDATION_LAYERS
    // Add validation layer for the instance creation itself
    VkDebugUtilsMessengerCreateInfoEXT validationLayerMessengerCreateInfo =
        this->getValidationLayerMessengerCreateInfo();
    instanceCreateInfo.pNext = &validationLayerMessengerCreateInfo;
#endif

    if (vkCreateInstance(&instanceCreateInfo, nullptr, &this->instance) != VK_SUCCESS) {
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

const VkInstance &Instance::getVkInstance() const noexcept {
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

std::vector<const char *> Instance::getExtensions() const {
    std::vector<const char *> extensions{};

    // Add GLFW extensions
    uint32_t glfwExtensionCount;
    auto glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    for (int i = 0; i < glfwExtensionCount; i++) {
        extensions.push_back(glfwExtensions[i]);
    }

#ifdef ENABLE_VALIDATION_LAYERS
    // Add extension for the validation layers message callback
    extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif

#ifdef DEBUG
    LOG_DEBUG("Available extensions: ");
    for (VkExtensionProperties &availableExtension : this->getAvailableExtensions()) {
        std::string availableExtensionName = static_cast<std::string>(availableExtension.extensionName);
        bool isUsed = false;
        for (auto &extension : extensions) {
            if (extension == availableExtensionName) {
                isUsed = true;
                break;
            }
        }
        LOG_DEBUG(std::string("  - ") + (isUsed ? "(used) " : "") + availableExtensionName);
    }
#endif

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

std::vector<const char *> Instance::getLayers() const {
    std::vector<const char *> layers{};

#ifdef ENABLE_VALIDATION_LAYERS
    layers.push_back("VK_LAYER_KHRONOS_validation");
#endif

#ifdef DEBUG
    LOG_DEBUG("Available layers: ");
    for (VkLayerProperties &availableLayer : this->getAvailableLayers()) {
        std::string availableLayerName = static_cast<std::string>(availableLayer.layerName);
        bool isUsed = false;
        for (auto &layer : layers) {
            if (layer == availableLayerName) {
                isUsed = true;
                break;
            }
        }
        LOG_DEBUG(std::string("  - ") + (isUsed ? "(used) " : "") + availableLayerName);
    }
#endif

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
