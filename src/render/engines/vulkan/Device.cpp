#include "Device.hpp"
#include "Instance.hpp"
#include "Swapchain.hpp"
#include <algorithm>
#include <cstdint>
#include <map>
#include <set>
#include <stdexcept>
#include <vulkan/vulkan_core.h>

Device::Device(const Instance &instance, const Surface &surface)
    : instance(instance),
      surface(surface) {
    // Choose the physical device
    this->physicalDevice = this->getBestPhysicalDevice();

    // Get the queue create infos
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    QueueFamilyIndices familyIndices = this->getQueueFamilyIndices(physicalDevice);
    std::set<uint32_t> uniqueQueueFamilies = {
        familyIndices.graphics.value(),
        familyIndices.present.value(),
    };
    float queuePriority = 1.0f;
    for (uint32_t familyIndex : uniqueQueueFamilies) {
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = familyIndex;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    // Get the device features
    VkPhysicalDeviceFeatures physicalDeviceFeatures{};

    // Get the extensions we want to use
    auto extensions = this->getExtensionsToUse(this->physicalDevice);

    // Create a VkDevice
    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.queueCreateInfoCount = queueCreateInfos.size();
    createInfo.pQueueCreateInfos = queueCreateInfos.data();
    createInfo.pEnabledFeatures = &physicalDeviceFeatures;
    createInfo.enabledExtensionCount = extensions.size();
    createInfo.ppEnabledExtensionNames = extensions.data();

    if (vkCreateDevice(this->physicalDevice, &createInfo, nullptr, &this->device) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create Vulkan device.");
    }

    // Retrieve the queues
    QueueFamilyIndices queueFamilyIndices = this->getQueueFamilyIndices(this->physicalDevice);
    vkGetDeviceQueue(this->device, queueFamilyIndices.graphics.value(), 0, &this->graphicsQueue);
    vkGetDeviceQueue(this->device, queueFamilyIndices.present.value(), 0, &this->presentQueue);
}

Device::~Device() {
    vkDestroyDevice(this->device, nullptr);
}

const VkDevice &Device::getVkDevice() const noexcept {
    return this->device;
}

const VkPhysicalDevice &Device::getVkPhysicalDevice() const noexcept {
    return this->physicalDevice;
}

QueueFamilyIndices Device::getQueueFamilyIndices() const {
    if (this->physicalDevice == VK_NULL_HANDLE) {
        throw std::runtime_error("Physical device has not been created yet.");
    }

    return this->getQueueFamilyIndices(this->physicalDevice);
}

VkQueue Device::getGraphicsQueue() const noexcept {
    return this->graphicsQueue;
}

VkQueue Device::getPresentQueue() const noexcept {
    return this->presentQueue;
}

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

    // Return the best physical device
    return scores.rbegin()->second;
}

uint32_t Device::ratePhysicalDevice(VkPhysicalDevice physicalDevice) const noexcept {
    uint32_t queueFamiliesScore = this->rateQueueFamilies(physicalDevice);
    uint32_t extensionsScore = this->rateExtensions(physicalDevice);
    uint32_t propertiesScore = this->rateProperties(physicalDevice);
    uint32_t swapchainScore = Swapchain::rate(physicalDevice, this->surface);

    return std::min({queueFamiliesScore, extensionsScore, propertiesScore}) == 0
               ? 0
               : queueFamiliesScore + extensionsScore + propertiesScore;
}

uint32_t Device::rateQueueFamilies(VkPhysicalDevice physicalDevice) const {
    QueueFamilyIndices queueFamilies = this->getQueueFamilyIndices(physicalDevice);

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

uint32_t Device::rateExtensions(VkPhysicalDevice physicalDevice) const {
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

uint32_t Device::rateProperties(VkPhysicalDevice physicalDevice) const noexcept {
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

QueueFamilyIndices Device::getQueueFamilyIndices(VkPhysicalDevice physicalDevice) const {
    QueueFamilyIndices indices;

    // Get the available queue families
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);
    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());

    uint32_t index = 0;
    for (auto &queueFamily : queueFamilies) {
        // Graphics
        if (!indices.graphics.has_value()) {
            if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                indices.graphics = index;
            }
        }

        // Present
        if (!indices.present.has_value()) {
            VkBool32 isPresentSupported;
            if (vkGetPhysicalDeviceSurfaceSupportKHR(
                    physicalDevice, index, this->surface.getVkSurface(), &isPresentSupported) != VK_SUCCESS) {
                throw std::runtime_error("Unable to check if a present queue is supported.");
            }
            if (isPresentSupported) {
                indices.present = index;
            }
        }

        ++index;
    }

    return indices;
}