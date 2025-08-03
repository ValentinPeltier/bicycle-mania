#include "VulkanEngine.hpp"
#include "core/Logger.hpp"
#include <GLFW/glfw3.h>
#include <stdexcept>

VulkanEngine::VulkanEngine() {
    this->instance = this->createInstance();

    LOG_DEBUG("Vulkan engine set up!");
}

VulkanEngine::~VulkanEngine() {
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

    LOG_DEBUG("Required extensions:");
    for (auto &extension : extensions) {
        LOG_DEBUG(std::string("  - ") + extension);
    }

    return extensions;
}