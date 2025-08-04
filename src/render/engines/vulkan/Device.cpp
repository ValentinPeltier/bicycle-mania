#include "Device.hpp"
#include "Instance.hpp"
#include "core/Logger.hpp"
#include <map>
#include <stdexcept>
#include <vulkan/vulkan_core.h>

Device::Device(const Instance &instance)
    : instance(instance) {
    // Choose the physical device
    VkPhysicalDevice physicalDevice = this->getBestPhysicalDevice();

    // Get the queue create info
    auto queueCreateInfo = this->getQueueCreateInfo(physicalDevice);

    // Get the device features
    VkPhysicalDeviceFeatures physicalDeviceFeatures{};

    // Create a VkDevice
    VkDeviceCreateInfo deviceCreateInfo{};
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.queueCreateInfoCount = queueCreateInfo.size();
    deviceCreateInfo.pQueueCreateInfos = queueCreateInfo.data();
    deviceCreateInfo.pEnabledFeatures = &physicalDeviceFeatures;
    deviceCreateInfo.enabledExtensionCount = 0;

    if (vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &this->device) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create Vulkan device.");
    }

    // Retrieve the queues
    auto queueIndices = Device::getQueueFamilies(physicalDevice);
    vkGetDeviceQueue(this->device, queueIndices.graphicsFamily.value(), 0, &this->graphicsQueue);
}

Device::~Device() {
    vkDestroyDevice(this->device, nullptr);
}

// -------------- //
// --- Device --- //
// -------------- //

uint32_t Device::ratePhysicalDevice(VkPhysicalDevice device) noexcept {
    uint32_t score = 0;

    // We need a device with a graphics queue family
    QueueFamilyIndices queueFamilies = Device::getQueueFamilies(device);
    if (!queueFamilies.graphicsFamily.has_value()) {
        return 0;
    }

    // Get the properties
    VkPhysicalDeviceProperties properties{};
    vkGetPhysicalDeviceProperties(device, &properties);

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

VkPhysicalDevice Device::getBestPhysicalDevice() const {
    // Enumerate physical devices
    uint32_t deviceCount;
    vkEnumeratePhysicalDevices(this->instance.getVkInstance(), &deviceCount, nullptr);
    if (deviceCount == 0) {
        throw std::runtime_error("No physical device available.");
    }
    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(this->instance.getVkInstance(), &deviceCount, devices.data());

    // Rate the physical devices
    // Use an ordered map to automatically sort candidates by increasing score
    std::multimap<uint32_t, VkPhysicalDevice> devicesByScore;
    for (const auto &device : devices) {
        uint32_t score = this->ratePhysicalDevice(device);
        devicesByScore.insert(std::make_pair(score, device));
    }

    // If the best physical device is not suitable
    if (devicesByScore.rbegin()->first == 0) {
        throw std::runtime_error("No suitable physical device.");
    }

    VkPhysicalDevice chosenDevice = devicesByScore.rbegin()->second;

#ifdef DEBUG
    LOG_DEBUG("Available physical devices:");
    for (auto deviceByScore : devicesByScore) {
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

// -------------- //
// --- Queues --- //
// -------------- //

QueueFamilyIndices Device::getQueueFamilies(VkPhysicalDevice physicalDevice) {
    QueueFamilyIndices indices;

    // Get the available queue families
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);
    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());

    uint32_t index = 0;
    for (auto &queueFamily : queueFamilies) {
        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.graphicsFamily = index;
        }

        ++index;
    }

    return indices;
}

std::vector<VkDeviceQueueCreateInfo> Device::getQueueCreateInfo(VkPhysicalDevice physicalDevice) const {
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfo{};
    QueueFamilyIndices familyIndices = Device::getQueueFamilies(physicalDevice);

    // Add graphics queue
    float graphicsQueuePriority = 1.0f;
    VkDeviceQueueCreateInfo graphicsQueueCreateInfo{};
    graphicsQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    graphicsQueueCreateInfo.queueFamilyIndex = familyIndices.graphicsFamily.value();
    graphicsQueueCreateInfo.queueCount = 1;
    graphicsQueueCreateInfo.pQueuePriorities = &graphicsQueuePriority;
    queueCreateInfo.push_back(graphicsQueueCreateInfo);

    return queueCreateInfo;
}