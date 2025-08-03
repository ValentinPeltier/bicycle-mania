#include "VulkanEngine.hpp"
#include "core/Logger.hpp"
#include "extensions.hpp"
#include <GLFW/glfw3.h>
#include <iostream>
#include <stdexcept>

VulkanEngine::VulkanEngine() {
    this->instance = this->createInstance();

    if (this->enableValidationLayers) {
        this->validationLayerMessenger = this->createValidationLayerMessenger();
    }

    this->device = this->createDevice();

    LOG_DEBUG("Vulkan engine set up!");
}

VulkanEngine::~VulkanEngine() {
    vkDestroyDevice(this->device, nullptr);
    vkDestroyDebugUtilsMessengerEXT(this->instance, this->validationLayerMessenger, nullptr);
    vkDestroyInstance(this->instance, nullptr);

    LOG_DEBUG("Vulkan engine cleaned up.");
}

// ------------------ //
// --- Instance --- //
// ------------------ //

VkInstance VulkanEngine::createInstance() const {
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

    // Add validation layer for the instance creation itself
    VkDebugUtilsMessengerCreateInfoEXT validationLayerMessengerCreateInfo{};
    if (this->enableValidationLayers) {
        validationLayerMessengerCreateInfo = this->getValidationLayerMessengerCreateInfo();
        instanceCreateInfo.pNext = &validationLayerMessengerCreateInfo;
    }

    VkInstance instance = VK_NULL_HANDLE;
    if (vkCreateInstance(&instanceCreateInfo, nullptr, &instance) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create Vulkan instance.");
    }

    return instance;
}

std::vector<VkExtensionProperties> VulkanEngine::getAvailableExtensions() const {
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

std::vector<const char *> VulkanEngine::getExtensions() const {
    std::vector<const char *> extensions{};

    // Add GLFW extensions
    uint32_t glfwExtensionCount;
    auto glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    for (int i = 0; i < glfwExtensionCount; i++) {
        extensions.push_back(glfwExtensions[i]);
    }

    // Add extension for the validation layers message callback
    if (this->enableValidationLayers) {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

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

std::vector<VkLayerProperties> VulkanEngine::getAvailableLayers() const {
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

std::vector<const char *> VulkanEngine::getLayers() const {
    std::vector<const char *> layers{};

    if (this->enableValidationLayers) {
        layers.push_back("VK_LAYER_KHRONOS_validation");
    }

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

VkDebugUtilsMessengerEXT VulkanEngine::createValidationLayerMessenger() const {
    VkDebugUtilsMessengerCreateInfoEXT createInfo = this->getValidationLayerMessengerCreateInfo();

    VkDebugUtilsMessengerEXT validationLayerMessenger = VK_NULL_HANDLE;
    if (vkCreateDebugUtilsMessengerEXT(this->instance, &createInfo, nullptr, &validationLayerMessenger) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create validation layer messenger.");
    }

    return validationLayerMessenger;
}

VkDebugUtilsMessengerCreateInfoEXT VulkanEngine::getValidationLayerMessengerCreateInfo() const noexcept {
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

VKAPI_ATTR VkBool32 VKAPI_CALL VulkanEngine::validationLayerMessageCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, void *pUserData) noexcept {

    if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
        LOG_ERROR(std::string("Validation layer: ") + pCallbackData->pMessage);
    } else if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
        LOG_WARNING(std::string("Validation layer: ") + pCallbackData->pMessage);
    }

    return VK_FALSE;
}

// -------------- //
// --- Device --- //
// -------------- //

VkDevice VulkanEngine::createDevice() const {
    // Choose the physical device
    VkPhysicalDevice physicalDevice = this->getPreferredPhysicalDevice();

    // Create a VkDevice
    VkDeviceCreateInfo deviceCreateInfo{};
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

    VkDevice device = VK_NULL_HANDLE;
    if (vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &device) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create Vulkan device.");
    }

    return device;
}

uint32_t VulkanEngine::ratePhysicalDevice(VkPhysicalDeviceProperties properties) const noexcept {
    uint32_t score = 0;

    // Prefer GPUs
    switch (properties.deviceType) {
        case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
            score += 1000;
            break;
        case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
        case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
            score += 200;
            break;
        default:
            break;
    }

    // Maximum possible size of textures affects graphics quality
    score += (int)(properties.limits.maxImageDimension2D / 1000);

    return score;
}

VkPhysicalDevice VulkanEngine::getPreferredPhysicalDevice() const {
    // Enumerate physical devices
    uint32_t physicalDeviceCount;
    vkEnumeratePhysicalDevices(this->instance, &physicalDeviceCount, nullptr);
    if (physicalDeviceCount == 0) {
        throw std::runtime_error("No physical device available.");
    }
    std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
    vkEnumeratePhysicalDevices(this->instance, &physicalDeviceCount, physicalDevices.data());

    // Rate the physical devices
    // Use an ordered map to automatically sort candidates by increasing score
    std::multimap<uint32_t, VkPhysicalDevice> physicalDevicesByScore;
    for (const auto &physicalDevice : physicalDevices) {
        // Get the physical device properties
        VkPhysicalDeviceProperties properties{};
        vkGetPhysicalDeviceProperties(physicalDevice, &properties);

        // Rate it
        uint32_t score = this->ratePhysicalDevice(properties);
        physicalDevicesByScore.insert(std::make_pair(score, physicalDevice));
    }

    // If the best physical device is not suitable
    if (physicalDevicesByScore.rbegin()->first == 0) {
        throw std::runtime_error("No suitable physical device.");
    }

    auto chosenDevice = physicalDevicesByScore.rbegin()->second;

#ifdef DEBUG
    LOG_DEBUG("Available physical devices:");
    for (auto physicalDeviceByScore : physicalDevicesByScore) {
        auto score = physicalDeviceByScore.first;
        auto physicalDevice = physicalDeviceByScore.second;
        bool isUsed = physicalDevice == chosenDevice;

        VkPhysicalDeviceProperties properties{};
        vkGetPhysicalDeviceProperties(physicalDevice, &properties);

        LOG_DEBUG(std::string("  - ") + (isUsed ? "(used) " : "") + "(score: " + std::to_string(score) + ") " +
                  properties.deviceName);
    }
#endif

    // Return the best physical device
    return chosenDevice;
}