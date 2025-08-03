#include "VulkanEngine.hpp"
#include "core/Logger.hpp"
#include "extensions.hpp"
#include <GLFW/glfw3.h>
#include <cstring>
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

void VulkanEngine::draw() const {
}

// ------------------ //
// --- Instance --- //
// ------------------ //

VkInstance VulkanEngine::createInstance() const {
    // Get required extensions
    auto extensions = this->getRequiredExtensions();

    // Get available extensions
    uint32_t availableExtensionCount;
    vkEnumerateInstanceExtensionProperties(nullptr, &availableExtensionCount, nullptr);
    std::vector<VkExtensionProperties> availableExtensions(availableExtensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &availableExtensionCount, availableExtensions.data());

    LOG_DEBUG("Available extensions: ");
    for (auto &availableExtension : availableExtensions) {
        LOG_DEBUG(std::string("  - ") + availableExtension.extensionName);
    }

    // Create a VkInstance
    VkApplicationInfo applicationInfo = {};
    applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    applicationInfo.pApplicationName = "GameTest";
    applicationInfo.applicationVersion = VK_MAKE_VERSION(0, 1, 0);
    applicationInfo.pEngineName = "No Engine";
    applicationInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    applicationInfo.apiVersion = VK_API_VERSION_1_4;

    VkInstanceCreateInfo instanceCreateInfo = {};
    instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceCreateInfo.pApplicationInfo = &applicationInfo;
    instanceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    instanceCreateInfo.ppEnabledExtensionNames = extensions.data();
    instanceCreateInfo.enabledLayerCount = 0;

    // Add validation layers
    VkDebugUtilsMessengerCreateInfoEXT validationLayerMessengerCreateInfo;
    if (this->enableValidationLayers) {
        if (!this->hasValidationLayersSupport()) {
            throw std::runtime_error("Validation layers are not available.");
        }

        instanceCreateInfo.enabledLayerCount = static_cast<uint32_t>(this->validationLayers.size());
        instanceCreateInfo.ppEnabledLayerNames = this->validationLayers.data();
        validationLayerMessengerCreateInfo = this->getValidationLayerMessengerCreateInfo();
        instanceCreateInfo.pNext = &validationLayerMessengerCreateInfo;
    }

    VkInstance instance = VK_NULL_HANDLE;
    if (vkCreateInstance(&instanceCreateInfo, nullptr, &instance) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create Vulkan instance.");
    }

    return instance;
}

std::vector<const char *> VulkanEngine::getRequiredExtensions() const noexcept {
    std::vector<const char *> extensions(0);

    // Add GLFW extensions
    uint32_t glfwExtensionCount;
    const char **glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    extensions.insert(extensions.end(), glfwExtensions, glfwExtensions + glfwExtensionCount);

    // Add extension for the message callback of validation layers
    if (this->enableValidationLayers) {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    LOG_DEBUG("Required extensions:");
    for (auto &extension : extensions) {
        LOG_DEBUG(std::string("  - ") + extension);
    }

    return extensions;
}

// ------------------------- //
// --- Validation layers --- //
// ------------------------- //

bool VulkanEngine::hasValidationLayersSupport() const noexcept {
    // Enumerate layer properties
    uint32_t availableLayerCount;
    vkEnumerateInstanceLayerProperties(&availableLayerCount, nullptr);
    std::vector<VkLayerProperties> availableLayers(availableLayerCount);
    vkEnumerateInstanceLayerProperties(&availableLayerCount, availableLayers.data());

    // Check that all layers are available
    for (const char *layerName : this->validationLayers) {
        bool layerFound = false;

        for (const auto &layerProperties : availableLayers) {
            if (std::strcmp(layerName, layerProperties.layerName) == 0) {
                layerFound = true;
                break;
            }
        }

        if (!layerFound) {
            return false;
        }
    }

    return true;
}

VkDebugUtilsMessengerEXT VulkanEngine::createValidationLayerMessenger() const {
    if (!this->hasValidationLayersSupport()) {
        throw std::runtime_error("Validation layers are not available.");
    }

    VkDebugUtilsMessengerCreateInfoEXT createInfo = this->getValidationLayerMessengerCreateInfo();

    VkDebugUtilsMessengerEXT validationLayerMessenger = VK_NULL_HANDLE;
    if (vkCreateDebugUtilsMessengerEXT(this->instance, &createInfo, nullptr, &validationLayerMessenger) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create validation layer messenger.");
    }

    return validationLayerMessenger;
}

VkDebugUtilsMessengerCreateInfoEXT VulkanEngine::getValidationLayerMessengerCreateInfo() const noexcept {
    VkDebugUtilsMessengerCreateInfoEXT createInfo = {};
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
    VkDeviceCreateInfo deviceCreateInfo = {};
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
    LOG_DEBUG("Available physical devices:");
    for (const auto &physicalDevice : physicalDevices) {
        // Get the physical device properties
        VkPhysicalDeviceProperties properties = {};
        vkGetPhysicalDeviceProperties(physicalDevice, &properties);

        // Rate it
        uint32_t score = this->ratePhysicalDevice(properties);
        physicalDevicesByScore.insert(std::make_pair(score, physicalDevice));

        LOG_DEBUG(std::string("  - ") + properties.deviceName + ": score " + std::to_string(score));
    }

    // If the best physical device is not suitable
    if (physicalDevicesByScore.rbegin()->first == 0) {
        throw std::runtime_error("No suitable physical device.");
    }

    // Return the best physical device
    return physicalDevicesByScore.rbegin()->second;
}