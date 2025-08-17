#include "Device.hpp"
#include "Instance.hpp"
#include "Swapchain.hpp"
#include <algorithm>
#include <array>
#include <cstdint>
#include <map>
#include <numeric>
#include <optional>
#include <set>
#include <stdexcept>
#include <vulkan/vulkan_core.h>

Device::Device(const Instance &instance, const Surface &surface)
    : instance(instance),
      surface(surface) {
    // Create device
    this->physicalDevice = this->getBestPhysicalDevice();
    this->device = this->createDevice();

    // Retrieve the queues
    this->graphicsQueueFamilyIndex = this->getGraphicsQueueFamilyIndex(this->physicalDevice).value();
    this->presentQueueFamilyIndex = this->getPresentQueueFamilyIndex(this->physicalDevice).value();
    vkGetDeviceQueue(this->device, this->graphicsQueueFamilyIndex, 0, &this->graphicsQueue);
    vkGetDeviceQueue(this->device, this->presentQueueFamilyIndex, 0, &this->presentQueue);

    // Create the graphics command pool
    this->graphicsCommandPool = this->createGraphicsCommandPool();
}

Device::~Device() {
    vkDestroyCommandPool(this->device, this->graphicsCommandPool, nullptr);

    vkDestroyDevice(this->device, nullptr);
}

VkDevice Device::getVkDevice() const noexcept {
    return this->device;
}

VkPhysicalDevice Device::getVkPhysicalDevice() const noexcept {
    return this->physicalDevice;
}

uint32_t Device::getGraphicsQueueFamilyIndex() const noexcept {
    return this->graphicsQueueFamilyIndex;
}

uint32_t Device::getPresentQueueFamilyIndex() const noexcept {
    return this->presentQueueFamilyIndex;
}

VkQueue Device::getGraphicsVkQueue() const noexcept {
    return this->graphicsQueue;
}

VkQueue Device::getPresentVkQueue() const noexcept {
    return this->presentQueue;
}

VkCommandPool Device::getGraphicsVkCommandPool() const noexcept {
    return this->graphicsCommandPool;
}

VkDevice Device::createDevice() const {
    // Get the queue create infos
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies = {
        this->getGraphicsQueueFamilyIndex(this->physicalDevice).value(),
        this->getPresentQueueFamilyIndex(this->physicalDevice).value(),
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

    VkDevice device;
    if (vkCreateDevice(this->physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create Vulkan device.");
    }

    return device;
}

VkCommandPool Device::createGraphicsCommandPool() const {
    VkCommandPoolCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    createInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    createInfo.queueFamilyIndex = this->graphicsQueueFamilyIndex;

    VkCommandPool graphicsCommandPool;
    if (vkCreateCommandPool(this->device, &createInfo, nullptr, &graphicsCommandPool) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create command pool.");
    }

    return graphicsCommandPool;
}

VkPhysicalDevice Device::getBestPhysicalDevice() const {
    // Get physical devices
    uint32_t physicalDeviceCount;
    if (vkEnumeratePhysicalDevices(this->instance.getVkInstance(), &physicalDeviceCount, nullptr) != VK_SUCCESS) {
        throw std::runtime_error("Unable to get the physical devices.");
    }
    std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
    if (vkEnumeratePhysicalDevices(this->instance.getVkInstance(), &physicalDeviceCount, physicalDevices.data()) !=
        VK_SUCCESS) {
        throw std::runtime_error("Unable to get the physical devices.");
    }
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
    std::array<uint32_t, 4> scores{
        this->rateQueueFamilies(physicalDevice),
        this->rateExtensions(physicalDevice),
        this->rateProperties(physicalDevice),
        Swapchain::rate(physicalDevice, this->surface),
    };

    // If one is zero, keep the total score to zero.
    // Otherwise, sum the intermediate scores.
    return std::ranges::min_element(scores) == 0 ? 0 : std::reduce(scores.begin(), scores.end());
}

uint32_t Device::rateQueueFamilies(VkPhysicalDevice physicalDevice) const {
    // We need a device with a graphics queue family
    if (!this->getGraphicsQueueFamilyIndex(physicalDevice)) {
        return 0;
    }

    // We need a device with a present queue family
    if (!this->getPresentQueueFamilyIndex(physicalDevice)) {
        return 0;
    }

    return 1;
}

uint32_t Device::rateExtensions(VkPhysicalDevice physicalDevice) const {
    // Get the available extensions
    auto extensions = this->getAvailableExtensions(physicalDevice);

    // Check the required extensions
    std::vector<std::string> missingExtensions{this->requiredExtensions.begin(), this->requiredExtensions.end()};
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
    std::vector<const char *> extensions{this->requiredExtensions.begin(), this->requiredExtensions.end()};

    // Add optional extensions (none for now)

    return extensions;
}

std::vector<VkQueueFamilyProperties> Device::getQueueFamilies(VkPhysicalDevice physicalDevice) const {
    uint32_t count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &count, nullptr);
    std::vector<VkQueueFamilyProperties> queueFamilies(count);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &count, queueFamilies.data());

    return queueFamilies;
}

std::optional<uint32_t> Device::getGraphicsQueueFamilyIndex(VkPhysicalDevice physicalDevice) const {
    const auto queueFamilies = this->getQueueFamilies(physicalDevice);

    for (uint32_t i = 0; i < queueFamilies.size(); ++i) {
        if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            return i;
        }
    }

    return {};
}

std::optional<uint32_t> Device::getPresentQueueFamilyIndex(VkPhysicalDevice physicalDevice) const {
    const auto queueFamilies = this->getQueueFamilies(physicalDevice);

    for (uint32_t i = 0; i < queueFamilies.size(); ++i) {
        VkBool32 isPresentSupported;
        if (vkGetPhysicalDeviceSurfaceSupportKHR(
                physicalDevice, i, this->surface.getVkSurface(), &isPresentSupported) != VK_SUCCESS) {
            throw std::runtime_error("Unable to check if a present queue is supported.");
        }
        if (isPresentSupported) {
            return i;
        }
    }

    return {};
}