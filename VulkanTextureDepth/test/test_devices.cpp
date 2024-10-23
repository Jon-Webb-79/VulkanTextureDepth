// ================================================================================
// ================================================================================
// - File:    queues.hpp
// - Purpose: Describe the file purpose here
//
// Source Metadata
// - Author:  Jonathan A. Webb
// - Date:    September 09, 2024
// - Version: 1.0
// - Copyright: Copyright 2024, Jon Webb Inc.
// ================================================================================
// ================================================================================
// Include modules here
#include <vulkan/vulkan.h>
#include <gtest/gtest.h>
#include <stdexcept>
#include <vector>
#include "../include/devices.hpp" 
#include "../include/queues.hpp" 
// ================================================================================
// ================================================================================
// Mocked device extensions
std::vector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
// --------------------------------------------------------------------------------
// Mocking function for Vulkan physical device enumeration

VkResult vkEnumeratePhysicalDevices(VkInstance instance, uint32_t* pDeviceCount, VkPhysicalDevice* pDevices) {
    if (pDeviceCount) {
        *pDeviceCount = 2;  // Mock that there are 2 devices available
    }
    if (pDevices) {
        pDevices[0] = reinterpret_cast<VkPhysicalDevice>(1);  // Mock physical device 1
        pDevices[1] = reinterpret_cast<VkPhysicalDevice>(2);  // Mock physical device 2
    }
    return VK_SUCCESS;
}
// --------------------------------------------------------------------------------
// Mocking function for Vulkan device extension enumeration

VkResult vkEnumerateDeviceExtensionProperties(VkPhysicalDevice device, const char* pLayerName, uint32_t* pPropertyCount, VkExtensionProperties* pProperties) {
    if (pPropertyCount) {
        *pPropertyCount = 1;  // Assume we have one extension
    }
    if (pProperties) {
        strncpy(pProperties[0].extensionName, VK_KHR_SWAPCHAIN_EXTENSION_NAME, VK_MAX_EXTENSION_NAME_SIZE);
    }
    return VK_SUCCESS;
}
// -------------------------------------------------------------------------------- 

class VulkanPhysicalDeviceTest : public ::testing::Test {
protected:
    VkInstance instance;
    VkSurfaceKHR surface;
    VulkanPhysicalDevice* physicalDevice;

    void SetUp() override {
        instance = reinterpret_cast<VkInstance>(1);  // Mock instance
        surface = reinterpret_cast<VkSurfaceKHR>(1); // Mock surface
        physicalDevice = nullptr;
    }

    void TearDown() override {
        delete physicalDevice;
    }
};
// ================================================================================
// ================================================================================
// Test that a valid physical device is selected

TEST_F(VulkanPhysicalDeviceTest, SelectsValidPhysicalDevice) {
    EXPECT_NO_THROW({
        physicalDevice = new VulkanPhysicalDevice(instance, surface);
    });

    EXPECT_NE(physicalDevice->getDevice(), VK_NULL_HANDLE);
}
// --------------------------------------------------------------------------------
// Test that an exception is thrown when no physical devices are found

TEST_F(VulkanPhysicalDeviceTest, ThrowsWhenNoPhysicalDevicesFound) {
    // Mock vkEnumeratePhysicalDevices to return no devices
    auto vkEnumeratePhysicalDevices_mock = [](VkInstance, uint32_t* pDeviceCount, VkPhysicalDevice*) -> VkResult {
        *pDeviceCount = 0;
        return VK_SUCCESS;
    };

    EXPECT_THROW({
        physicalDevice = new VulkanPhysicalDevice(instance, surface);
    }, std::runtime_error);
}
// -------------------------------------------------------------------------------- 
// Test that getDevice returns a valid physical device after successful initialization

TEST_F(VulkanPhysicalDeviceTest, GetDeviceReturnsValidPhysicalDevice) {
    physicalDevice = new VulkanPhysicalDevice(instance, surface);
    EXPECT_NE(physicalDevice->getDevice(), VK_NULL_HANDLE);
}
// --------------------------------------------------------------------------------
// Test that getDevice returns VK_NULL_HANDLE if no suitable device is found

TEST_F(VulkanPhysicalDeviceTest, GetDeviceReturnsNullIfNoSuitableDevice) {
    // Mock vkEnumeratePhysicalDevices to return devices that are not suitable
    auto isDeviceSuitable_mock = [](VkPhysicalDevice) -> bool {
        return false;
    };

    EXPECT_THROW({
        physicalDevice = new VulkanPhysicalDevice(instance, surface);
    }, std::runtime_error);

    if (physicalDevice) {
        EXPECT_EQ(physicalDevice->getDevice(), VK_NULL_HANDLE);
    }
}
// ================================================================================
// ================================================================================
// eof
