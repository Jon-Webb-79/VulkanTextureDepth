// ================================================================================
// ================================================================================
// - File:    devices.cpp
// - Purpose: Describe the file purpose here
//
// Source Metadata
// - Author:  Jonathan A. Webb
// - Date:    June 22, 2024
// - Version: 1.0
// - Copyright: Copyright 2022, Jon Webb Inc.
// ================================================================================
// ================================================================================
// Include modules here

#include "include/devices.hpp"
#include "include/queues.hpp"
#include "include/constants.hpp"
#include <stdexcept>
#include <vector>
#include <set>
#include <limits>
#include <algorithm>
#include <iostream>
// ================================================================================
// ================================================================================

VulkanPhysicalDevice::VulkanPhysicalDevice(VkInstance& instance, VkSurfaceKHR surface)
    : instance(instance), surface(surface) {

    std::lock_guard<std::mutex> lock(deviceMutex);  // Lock when setting physicalDevice

    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

    if (deviceCount == 0) {
        throw std::runtime_error("Failed to find GPUs with Vulkan support!");
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

    int bestScore = -1;  // Initialize bestScore to a low value

    for (const auto& device : devices) {
        int score = rateDeviceSuitability(device);  // Calculate the score of the device
        if (score > bestScore && isDeviceSuitable(device)) {  // Choose the best device
            bestScore = score;
            physicalDevice = device;
        }
    }

    if (physicalDevice == VK_NULL_HANDLE) {
        throw std::runtime_error("Failed to find a suitable GPU!");
    }
}
// --------------------------------------------------------------------------------

VulkanPhysicalDevice::VulkanPhysicalDevice(VulkanPhysicalDevice&& other) noexcept
    : instance(other.instance), surface(other.surface), physicalDevice(other.physicalDevice) {
    std::lock_guard<std::mutex> lock(other.deviceMutex);  // Lock the source mutex before transferring

    // Transfer ownership of the physical device
    other.physicalDevice = VK_NULL_HANDLE;  // Reset the source object
}
// --------------------------------------------------------------------------------

VulkanPhysicalDevice& VulkanPhysicalDevice::operator=(VulkanPhysicalDevice&& other) noexcept {
    if (this != &other) {
        std::lock_guard<std::mutex> lock(deviceMutex);       // Lock this object's mutex
        std::lock_guard<std::mutex> otherLock(other.deviceMutex);  // Lock the source object's mutex

        instance = other.instance;
        surface = other.surface;
        physicalDevice = other.physicalDevice;

        // Reset the source object
        other.physicalDevice = VK_NULL_HANDLE;
    }
    return *this;
}
// --------------------------------------------------------------------------------

const VkPhysicalDevice VulkanPhysicalDevice::getDevice() const {
    std::lock_guard<std::mutex> lock(deviceMutex);
    return physicalDevice;
}
// --------------------------------------------------------------------------------

bool VulkanPhysicalDevice::isDeviceSuitable(const VkPhysicalDevice device) const {
    QueueFamilyIndices indices = QueueFamily::findQueueFamilies(device, surface);

    bool extensionsSupported = checkDeviceExtensionSupport(device);

    bool swapChainAdequate = false;
    if (extensionsSupported) {
        SwapChainSupportDetails swapChainSupport = SwapChain::querySwapChainSupport(device, surface);
        swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
    }

    VkPhysicalDeviceFeatures supportedFeatures;
    vkGetPhysicalDeviceFeatures(device, &supportedFeatures);

    return indices.isComplete() && extensionsSupported && swapChainAdequate && supportedFeatures.samplerAnisotropy;
}
// --------------------------------------------------------------------------------

bool VulkanPhysicalDevice::checkDeviceExtensionSupport(const VkPhysicalDevice& device) const {
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

    std::set<std::string> availableExtensionSet;
    for (const auto& extension : availableExtensions) {
        availableExtensionSet.insert(extension.extensionName);
    }

    for (const char* const& required : deviceExtensions) {  // Use const char* const& to prevent temporary construction
        if (availableExtensionSet.find(required) == availableExtensionSet.end()) {
            return false;  // Missing required extension
        }
    }

    return true;  // All required extensions are supported
}
// ================================================================================

int VulkanPhysicalDevice::rateDeviceSuitability(const VkPhysicalDevice device) {
    int score = 0;

    VkPhysicalDeviceProperties deviceProperties;
    vkGetPhysicalDeviceProperties(device, &deviceProperties);

    // Discrete GPUs have a performance advantage
    if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
        score += 1000;
    }

    // Maximum possible size of textures affects graphics quality
    score += deviceProperties.limits.maxImageDimension2D;

    return score;
}
// ================================================================================
// ================================================================================


VulkanLogicalDevice::VulkanLogicalDevice(VkPhysicalDevice physicalDevice, 
                                         const std::vector<const char*>& validationLayers,
                                         VkSurfaceKHR surface,
                                         const std::vector<const char*>& deviceExtensions)
    : physicalDevice(physicalDevice), 
      validationLayers(validationLayers),
      surface(surface),
      deviceExtensions(deviceExtensions) {
    createLogicalDevice();
}

// --------------------------------------------------------------------------------

VulkanLogicalDevice::~VulkanLogicalDevice() {
    std::lock_guard<std::mutex> lock(deviceMutex); // Protect device destruction
    if (device != VK_NULL_HANDLE) {
        vkDestroyDevice(device, nullptr);
        device = VK_NULL_HANDLE; // Reset to a known state
    }
}

// --------------------------------------------------------------------------------

VkDevice VulkanLogicalDevice::getDevice() const {
    std::lock_guard<std::mutex> lock(deviceMutex);
    return device;
}

// --------------------------------------------------------------------------------

VkQueue VulkanLogicalDevice::getGraphicsQueue() const {
    std::lock_guard<std::mutex> lock(queueMutex);
    return graphicsQueue;
}

// --------------------------------------------------------------------------------

VkQueue VulkanLogicalDevice::getPresentQueue() const {
    std::lock_guard<std::mutex> lock(queueMutex);
    return presentQueue;
}

// --------------------------------------------------------------------------------

void VulkanLogicalDevice::createLogicalDevice() {
    QueueFamilyIndices indices = QueueFamily::findQueueFamilies(physicalDevice, surface);

    if (!indices.graphicsFamily.has_value() || !indices.presentFamily.has_value()) {
        throw std::runtime_error("Failed to find required queue families.");
    }

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily.value(), indices.presentFamily.value()};

    float queuePriority = 1.0f;
    for (uint32_t queueFamily : uniqueQueueFamilies) {
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    VkPhysicalDeviceFeatures deviceFeatures{};
    deviceFeatures.samplerAnisotropy = VK_TRUE;

    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    createInfo.pQueueCreateInfos = queueCreateInfos.data();
    createInfo.pEnabledFeatures = &deviceFeatures;

    createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
    createInfo.ppEnabledExtensionNames = deviceExtensions.data();

    if (!validationLayers.empty()) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();
    } else {
        createInfo.enabledLayerCount = 0;
    }

    {
        std::lock_guard<std::mutex> lock(deviceMutex); // Lock while creating the device
        if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create logical device!");
        }
    }

    {
        std::lock_guard<std::mutex> lock(queueMutex); // Lock while accessing the queues
        vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &graphicsQueue);
        vkGetDeviceQueue(device, indices.presentFamily.value(), 0, &presentQueue);
    }

    std::cout << "Logical device and queues created successfully." << std::endl; // For logging
}

// --------------------------------------------------------------------------------

VulkanLogicalDevice::VulkanLogicalDevice(VulkanLogicalDevice&& other) noexcept
    : device(VK_NULL_HANDLE), graphicsQueue(VK_NULL_HANDLE), presentQueue(VK_NULL_HANDLE),
      physicalDevice(other.physicalDevice), validationLayers(std::move(other.validationLayers)),
      surface(other.surface), deviceExtensions(std::move(other.deviceExtensions)) {

    std::lock_guard<std::mutex> lock(other.deviceMutex); // Lock other object's mutex

    device = other.device;
    graphicsQueue = other.graphicsQueue;
    presentQueue = other.presentQueue;

    // Reset the source object
    other.device = VK_NULL_HANDLE;
}

// --------------------------------------------------------------------------------

VulkanLogicalDevice& VulkanLogicalDevice::operator=(VulkanLogicalDevice&& other) noexcept {
    if (this != &other) {
        std::lock_guard<std::mutex> lockThis(deviceMutex);
        std::lock_guard<std::mutex> lockOther(other.deviceMutex);

        // Destroy current device if exists
        if (device != VK_NULL_HANDLE) {
            vkDestroyDevice(device, nullptr);
        }

        device = other.device;
        graphicsQueue = other.graphicsQueue;
        presentQueue = other.presentQueue;
        physicalDevice = other.physicalDevice;
        validationLayers = std::move(other.validationLayers); // Move the vectors
        deviceExtensions = std::move(other.deviceExtensions);
        surface = other.surface;

        // Reset the source object
        other.device = VK_NULL_HANDLE;
    }
    return *this;
}
// ================================================================================
// ================================================================================


SwapChain::SwapChain(VkDevice device, 
                     VkSurfaceKHR surface, 
                     VkPhysicalDevice physicalDevice, 
                     GLFWwindow* window)
    : device(device), 
      surface(surface), 
      physicalDevice(physicalDevice),
      window(window) {
    createSwapChain();
    createImageViews();
}
// --------------------------------------------------------------------------------

SwapChain::~SwapChain() {
    cleanupImageViews();
    vkDestroySwapchainKHR(device, swapChain, nullptr);
}
// --------------------------------------------------------------------------------

VkSwapchainKHR SwapChain::getSwapChain() const {
    return swapChain;
}
// --------------------------------------------------------------------------------

VkFormat SwapChain::getSwapChainImageFormat() const {
    return swapChainImageFormat;
}
// --------------------------------------------------------------------------------

VkExtent2D SwapChain::getSwapChainExtent() const {
    return swapChainExtent;
}
// --------------------------------------------------------------------------------

const std::vector<VkImage>& SwapChain::getSwapChainImages() const {
    return swapChainImages;
}
// --------------------------------------------------------------------------------

const std::vector<VkImageView>& SwapChain::getSwapChainImageViews() const {
    return swapChainImageViews;
}
// --------------------------------------------------------------------------------

SwapChainSupportDetails SwapChain::querySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface) {
    SwapChainSupportDetails details;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

    if (formatCount != 0) {
        details.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
    }

    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);

    if (presentModeCount != 0) {
        details.presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
    }

    return details;
}
// --------------------------------------------------------------------------------

void SwapChain::cleanupSwapChain() {
    // Destroy the existing swap chain-related resources
    for (auto imageView : swapChainImageViews) {
        vkDestroyImageView(device, imageView, nullptr);
    }

    vkDestroySwapchainKHR(device, swapChain, nullptr);
}
// --------------------------------------------------------------------------------

void SwapChain::recreateSwapChain() {
    // Logic to recreate the swap chain, similar to what was done during the initial creation
    createSwapChain();  // Recreate the swap chain with updated parameters (like new window size)
    createImageViews();  // Recreate the image views for the swap chain images
}

// ================================================================================

void SwapChain::createSwapChain() {
    SwapChainSupportDetails swapChainSupport = querySwapChainSupport(physicalDevice, surface);

    VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
    VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
    VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
    if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
        imageCount = swapChainSupport.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = surface;

    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    QueueFamilyIndices indices = QueueFamily::findQueueFamilies(physicalDevice, surface);
    uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(), indices.presentFamily.value()};

    if (indices.graphicsFamily != indices.presentFamily) {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    } else {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }

    createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = VK_NULL_HANDLE;

    if (vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapChain) != VK_SUCCESS) {
        throw std::runtime_error("failed to create swap chain!");
    }

    vkGetSwapchainImagesKHR(device, swapChain, &imageCount, nullptr);
    swapChainImages.resize(imageCount);
    vkGetSwapchainImagesKHR(device, swapChain, &imageCount, swapChainImages.data());

    swapChainImageFormat = surfaceFormat.format;
    swapChainExtent = extent;
}
// --------------------------------------------------------------------------------

void SwapChain::createImageViews() {
    swapChainImageViews.resize(swapChainImages.size());
    for (size_t i = 0; i < swapChainImages.size(); i++) {
        swapChainImageViews[i] = createImageView(swapChainImages[i], swapChainImageFormat);
    }
}
// void SwapChain::createImageViews() {
//     swapChainImageViews.resize(swapChainImages.size());
//
//     for (size_t i = 0; i < swapChainImages.size(); i++) {
//         VkImageViewCreateInfo createInfo{};
//         createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
//         createInfo.image = swapChainImages[i];
//         createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
//         createInfo.format = swapChainImageFormat;
//         createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
//         createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
//         createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
//         createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
//         createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
//         createInfo.subresourceRange.baseMipLevel = 0;
//         createInfo.subresourceRange.levelCount = 1;
//         createInfo.subresourceRange.baseArrayLayer = 0;
//         createInfo.subresourceRange.layerCount = 1;
//
//         if (vkCreateImageView(device, &createInfo, nullptr, &swapChainImageViews[i]) != VK_SUCCESS) {
//             throw std::runtime_error("failed to create image views!");
//         }
//     }
// }
// --------------------------------------------------------------------------------

VkImageView SwapChain::createImageView(VkImage image, VkFormat format) {
    VkImageViewCreateInfo viewInfo{};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = image;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = format;
    viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    VkImageView imageView;
    if (vkCreateImageView(device, &viewInfo, nullptr, &imageView) != VK_SUCCESS) {
        throw std::runtime_error("failed to create image views for swap chain!");
    }

    return imageView;
}
// --------------------------------------------------------------------------------

void SwapChain::cleanupImageViews() {
    for (auto imageView : swapChainImageViews) {
        vkDestroyImageView(device, imageView, nullptr);
    }
}
// --------------------------------------------------------------------------------

VkSurfaceFormatKHR SwapChain::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
    for (const auto& availableFormat : availableFormats) {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return availableFormat;
        }
    }

    return availableFormats[0];
}
// --------------------------------------------------------------------------------

VkPresentModeKHR SwapChain::chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) {
    for (const auto& availablePresentMode : availablePresentModes) {
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
            return availablePresentMode;
        }
    }

    return VK_PRESENT_MODE_FIFO_KHR;
}
// --------------------------------------------------------------------------------

VkExtent2D SwapChain::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) {
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
        return capabilities.currentExtent;
    } else {
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);

        VkExtent2D actualExtent = {
            static_cast<uint32_t>(width), 
            static_cast<uint32_t>(height)
        };

        actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

        return actualExtent;
    }
}
// ================================================================================
// ================================================================================
// eof
