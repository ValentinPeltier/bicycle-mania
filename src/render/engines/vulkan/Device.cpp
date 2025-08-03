#include "Device.hpp"
#include "Instance.hpp"
#include "core/Logger.hpp"
#include <map>
#include <stdexcept>

Device::Device(const Instance &instance)
    : instance(instance) {
    // Choose the physical device
    VkPhysicalDevice physicalDevice = this->getBestPhysicalDevice();

    // Create a VkDevice
    VkDeviceCreateInfo deviceCreateInfo{};
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

    if (vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &this->device) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create Vulkan device.");
    }
}

Device::~Device() {
    vkDestroyDevice(this->device, nullptr);
}

// -------------- //
// --- Device --- //
// -------------- //

uint32_t Device::ratePhysicalDevice(VkPhysicalDeviceProperties properties) const noexcept {
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

VkPhysicalDevice Device::getBestPhysicalDevice() const {
    // Enumerate physical devices
    uint32_t deviceCount;
    vkEnumeratePhysicalDevices(this->instance.get(), &deviceCount, nullptr);
    if (deviceCount == 0) {
        throw std::runtime_error("No physical device available.");
    }
    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(this->instance.get(), &deviceCount, devices.data());

    // Rate the physical devices
    // Use an ordered map to automatically sort candidates by increasing score
    std::multimap<uint32_t, VkPhysicalDevice> devicesByScore;
    for (const auto &device : devices) {
        // Get the physical device properties
        VkPhysicalDeviceProperties properties{};
        vkGetPhysicalDeviceProperties(device, &properties);

        // Rate it
        uint32_t score = this->ratePhysicalDevice(properties);
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