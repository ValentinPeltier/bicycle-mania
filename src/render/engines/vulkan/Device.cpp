#include "Device.hpp"
#include "Instance.hpp"
#include "core/Logger.hpp"
#include <algorithm>
#include <map>
#include <stdexcept>
#include <vulkan/vulkan_core.h>

Device::Device(const Instance &instance, const Surface &surface)
    : instance(instance),
      surface(surface) {
    // Choose the physical device
    VkPhysicalDevice physicalDevice = this->getBestPhysicalDevice();

    // Get the queue create info
    auto queueCreateInfos = this->getPhysicalDeviceQueueCreateInfos(physicalDevice);

    // Get the device features
    VkPhysicalDeviceFeatures physicalDeviceFeatures{};

    // Get the extensions we want to use
    auto extensions = this->getExtensionsToUse(physicalDevice);

    // Create a VkDevice
    VkDeviceCreateInfo deviceCreateInfo{};
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.queueCreateInfoCount = queueCreateInfos.size();
    deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
    deviceCreateInfo.pEnabledFeatures = &physicalDeviceFeatures;
    deviceCreateInfo.enabledExtensionCount = extensions.size();
    deviceCreateInfo.ppEnabledExtensionNames = extensions.data();

    if (vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &this->device) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create Vulkan device.");
    }

    // Retrieve the queues
    QueueFamilyIndices queueFamilyIndices = this->getPhysicalDeviceQueueFamilies(physicalDevice);
    vkGetDeviceQueue(this->device, queueFamilyIndices.graphics.value(), 0, &this->graphicsQueue);
    vkGetDeviceQueue(this->device, queueFamilyIndices.present.value(), 0, &this->presentQueue);
}

Device::~Device() {
    vkDestroyDevice(this->device, nullptr);
}

// ----------------------- //
// --- Physical device --- //
// ----------------------- //

std::vector<VkPhysicalDevice> Device::getPhysicalDevices() const {
    uint32_t physicalDeviceCount;
    if (vkEnumeratePhysicalDevices(this->instance.getVkInstance(), &physicalDeviceCount, nullptr) != VK_SUCCESS) {
        throw std::runtime_error("Unable to get the physical devices.");
    }
    std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
    if (vkEnumeratePhysicalDevices(this->instance.getVkInstance(), &physicalDeviceCount, physicalDevices.data()) !=
        VK_SUCCESS) {
        throw std::runtime_error("Unable to get the physical devices.");
    }

    return physicalDevices;
}

VkPhysicalDevice Device::getBestPhysicalDevice() const {
    // Get physical devices
    auto physicalDevices = this->getPhysicalDevices();
    if (physicalDevices.empty()) {
        throw std::runtime_error("No physical device available.");
    }

    // Rate the physical devices
    // Use an ordered map to automatically sort candidates by increasing score
    std::multimap<uint32_t, VkPhysicalDevice> scores;
    for (const auto &physicalDevice : physicalDevices) {
        uint32_t score = this->ratePhysicalDevice(physicalDevice);
        scores.insert(std::make_pair(score, physicalDevice));
    }

    // If the best physical device is not suitable
    if (scores.rbegin()->first == 0) {
        throw std::runtime_error("No suitable physical device.");
    }

    VkPhysicalDevice chosenDevice = scores.rbegin()->second;

#ifdef DEBUG
    LOG_DEBUG("Available physical devices:");
    for (auto deviceByScore : scores) {
        bool isUsed = deviceByScore.second == chosenDevice;

        VkPhysicalDeviceProperties properties{};
        vkGetPhysicalDeviceProperties(deviceByScore.second, &properties);

        LOG_DEBUG(std::string("  - ") + (isUsed ? "(used) " : "") + "(score: " + std::to_string(deviceByScore.first) +
                  ") " + properties.deviceName);
    }
#endif

    // Return the best physical device
    return chosenDevice;
}

uint32_t Device::ratePhysicalDevice(VkPhysicalDevice physicalDevice) const noexcept {
    uint32_t queueFamiliesScore = this->ratePhysicalDeviceQueueFamilies(physicalDevice);
    uint32_t extensionsScore = this->ratePhysicalDeviceExtensions(physicalDevice);
    uint32_t propertiesScore = this->ratePhysicalDeviceProperties(physicalDevice);

    return std::min({queueFamiliesScore, extensionsScore, propertiesScore}) == 0
               ? 0
               : queueFamiliesScore + extensionsScore + propertiesScore;
}

uint32_t Device::ratePhysicalDeviceQueueFamilies(VkPhysicalDevice physicalDevice) const noexcept {
    QueueFamilyIndices queueFamilies = this->getPhysicalDeviceQueueFamilies(physicalDevice);

    // We need a device with a graphics queue family
    if (!queueFamilies.graphics.has_value()) {
        return 0;
    }

    // We need a device with a present queue family
    if (!queueFamilies.present.has_value()) {
        return 0;
    }

    return 1;
}

uint32_t Device::ratePhysicalDeviceExtensions(VkPhysicalDevice physicalDevice) const noexcept {
    // Get the available extensions
    auto extensions = this->getAvailableExtensions(physicalDevice);

    // Check the required extensions
    std::vector<std::string> missingExtensions(this->requiredExtensions.begin(), this->requiredExtensions.end());
    for (auto &extension : extensions) {
        std::erase(missingExtensions, extension.extensionName);
    }
    if (!missingExtensions.empty()) {
        return 0;
    }

    return 1;
}

uint32_t Device::ratePhysicalDeviceProperties(VkPhysicalDevice physicalDevice) const noexcept {
    uint32_t score = 0;

    VkPhysicalDeviceProperties properties{};
    vkGetPhysicalDeviceProperties(physicalDevice, &properties);

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
            score += 1; // Any type of device will do the job, never return score 0
            break;
    }

    // Prefer higher image dimensions
    score += (int)(properties.limits.maxImageDimension2D / 1000);

    return score;
}

std::vector<VkExtensionProperties> Device::getAvailableExtensions(VkPhysicalDevice physicalDevice) const {
    uint32_t extensionCount;
    if (vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, nullptr) != VK_SUCCESS) {
        throw std::runtime_error("Unable to get the available device extensions.");
    }
    std::vector<VkExtensionProperties> extensions(extensionCount);
    if (vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, extensions.data()) !=
        VK_SUCCESS) {
        throw std::runtime_error("Unable to get the available device extensions.");
    }

    return extensions;
}

std::vector<const char *> Device::getExtensionsToUse(VkPhysicalDevice physicalDevice) const {
    // Start with the required extensions
    std::vector<const char *> extensions = this->requiredExtensions;

    // Add optional extensions (none for now)

    return extensions;
}

// -------------- //
// --- Queues --- //
// -------------- //

QueueFamilyIndices Device::getPhysicalDeviceQueueFamilies(VkPhysicalDevice physicalDevice) const noexcept {
    QueueFamilyIndices indices;

    // Get the available queue families
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);
    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());

    uint32_t index = 0;
    for (auto &queueFamily : queueFamilies) {
        // Graphics
        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.graphics = index;
        }

        // Present
        VkBool32 isPresentSupported;
        vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, index, this->surface.getVkSurface(), &isPresentSupported);
        if (isPresentSupported) {
            indices.present = index;
        }

        ++index;
    }

    return indices;
}

std::vector<VkDeviceQueueCreateInfo> Device::getPhysicalDeviceQueueCreateInfos(VkPhysicalDevice physicalDevice) const {
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfo{};
    QueueFamilyIndices familyIndices = this->getPhysicalDeviceQueueFamilies(physicalDevice);

    // Add graphics queue
    float graphicsQueuePriority = 1.0f;
    VkDeviceQueueCreateInfo graphicsQueueCreateInfo{};
    graphicsQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    graphicsQueueCreateInfo.queueFamilyIndex = familyIndices.graphics.value();
    graphicsQueueCreateInfo.queueCount = 1;
    graphicsQueueCreateInfo.pQueuePriorities = &graphicsQueuePriority;
    queueCreateInfo.push_back(graphicsQueueCreateInfo);

    // Add present queue
    float presentQueuePriority = 1.0f;
    VkDeviceQueueCreateInfo presentQueueCreateInfo{};
    presentQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    presentQueueCreateInfo.queueFamilyIndex = familyIndices.present.value();
    presentQueueCreateInfo.queueCount = 1;
    presentQueueCreateInfo.pQueuePriorities = &presentQueuePriority;
    queueCreateInfo.push_back(presentQueueCreateInfo);

    return queueCreateInfo;
}