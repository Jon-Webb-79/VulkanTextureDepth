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

#include <gtest/gtest.h>
#include <vulkan/vulkan.h>
#include "../include/validation_layers.hpp"
#include <vector>
#include <cstring>
#include <iostream>
// ================================================================================
// ================================================================================
// Mocking the Vulkan function vkEnumerateInstanceLayerProperties

VkResult vkEnumerateInstanceLayerProperties(uint32_t* pLayerCount, VkLayerProperties* pProperties) {
    if (pLayerCount) {
        *pLayerCount = 1;  // Assume there is 1 layer available
    }
    if (pProperties) {
        strcpy(pProperties[0].layerName, "VK_LAYER_KHRONOS_validation");
    }
    return VK_SUCCESS;
}
// -------------------------------------------------------------------------------- 
// Mocking vkGetInstanceProcAddr to return a mock vkCreateDebugUtilsMessengerEXT

PFN_vkVoidFunction vkGetInstanceProcAddr(VkInstance instance, const char* pName) {
    if (strcmp(pName, "vkCreateDebugUtilsMessengerEXT") == 0) {
        return reinterpret_cast<PFN_vkVoidFunction>(+[](VkInstance, const VkDebugUtilsMessengerCreateInfoEXT*, const VkAllocationCallbacks*, VkDebugUtilsMessengerEXT* pDebugMessenger) {
            *pDebugMessenger = reinterpret_cast<VkDebugUtilsMessengerEXT>(1); // Mock success
            return VK_SUCCESS;
        });
    }
    return nullptr;
}
// -------------------------------------------------------------------------------- 
// Mocking vkDestroyDebugUtilsMessengerEXT for completeness

void vkDestroyDebugUtilsMessengerEXT(VkInstance, VkDebugUtilsMessengerEXT, const VkAllocationCallbacks*) {
    // Mock destroy function; nothing to do here
}
// --------------------------------------------------------------------------------
// Test suite for ValidationLayers

class ValidationLayersTest : public ::testing::Test {
protected:
    ValidationLayers* validationLayers;

    void SetUp() override {
        validationLayers = new ValidationLayers();
    }

    void TearDown() override {
        delete validationLayers;
    }
};
// ================================================================================ 
// ================================================================================ 
// Test that validation layers are enabled when not in NDEBUG mode

TEST_F(ValidationLayersTest, ValidationLayersEnabled) {
    EXPECT_TRUE(validationLayers->isEnabled());
}
// -------------------------------------------------------------------------------- 
// Test that the required extensions include the debug utils extension

TEST_F(ValidationLayersTest, RequiredExtensionsIncludesDebugUtils) {
    auto extensions = validationLayers->getRequiredExtensions();
    
    bool foundDebugUtils = false;
    for (const auto& ext : extensions) {
        if (strcmp(ext, VK_EXT_DEBUG_UTILS_EXTENSION_NAME) == 0) {
            foundDebugUtils = true;
            break;
        }
    }

    EXPECT_TRUE(foundDebugUtils);
}
// -------------------------------------------------------------------------------- 
// Test that the validation layer support is correctly checked

TEST_F(ValidationLayersTest, CheckValidationLayerSupport) {
    EXPECT_TRUE(validationLayers->checkValidationLayerSupport());
}
// -------------------------------------------------------------------------------- 
// Test that the debug messenger can be set up correctly

TEST_F(ValidationLayersTest, SetupDebugMessengerSuccess) {
    VkInstance instance = reinterpret_cast<VkInstance>(1);  // Mock Vulkan instance
    EXPECT_NO_THROW(validationLayers->setupDebugMessenger(instance));
}
// --------------------------------------------------------------------------------
// Test that the cleanup works without any issues

TEST_F(ValidationLayersTest, CleanupWorks) {
    VkInstance instance = reinterpret_cast<VkInstance>(1);  // Mock Vulkan instance
    EXPECT_NO_THROW(validationLayers->cleanup(instance));
}
// -------------------------------------------------------------------------------- 
// Test that the debug messenger handle is returned correctly

TEST_F(ValidationLayersTest, GetDebugMessenger) {
    VkInstance instance = reinterpret_cast<VkInstance>(1);  // Mock Vulkan instance
    validationLayers->setupDebugMessenger(instance);
    EXPECT_NE(validationLayers->getDebugMessenger(), VK_NULL_HANDLE);
}
// ================================================================================
// ================================================================================
// eof
