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
#include <GLFW/glfw3.h>
#include <gmock/gmock.h>
#include "../include/application.hpp" // Assuming the VulkanInstance class is here
#include "../include/validation_layers.hpp" 

// Mock class for VulkanGLFWLoader
class MockVulkanGLFWLoader : public VulkanGLFWLoader {
public:
    // Correct function signatures based on Vulkan and GLFW types
    MOCK_METHOD(VkResult, vkCreateInstance, (const VkInstanceCreateInfo*, const VkAllocationCallbacks*, VkInstance*), (override));
    MOCK_METHOD(void, vkDestroyInstance, (VkInstance, const VkAllocationCallbacks*), (override));
    MOCK_METHOD(VkResult, glfwCreateWindowSurface, (VkInstance, GLFWwindow*, const VkAllocationCallbacks*, VkSurfaceKHR*), (override));
};
// Test class
class VulkanInstanceTest : public ::testing::Test {
protected:
    GLFWwindow* mockWindow;
    ValidationLayers validationLayers;
    MockVulkanGLFWLoader mockLoader;  // Use the mock loader
    VulkanInstance* vulkanInstance;

    void SetUp() override {
        // Mock a GLFW window
        mockWindow = reinterpret_cast<GLFWwindow*>(1);

        // Expect the mock Vulkan and GLFW functions to succeed
        EXPECT_CALL(mockLoader, vkCreateInstance)
            .WillOnce([](const VkInstanceCreateInfo*, const VkAllocationCallbacks*, VkInstance* instance) {
                *instance = reinterpret_cast<VkInstance>(1);  // Mock a valid Vulkan instance
                return VK_SUCCESS;
            });

        EXPECT_CALL(mockLoader, glfwCreateWindowSurface)
            .WillOnce([](VkInstance, GLFWwindow*, const VkAllocationCallbacks*, VkSurfaceKHR* surface) {
                *surface = reinterpret_cast<VkSurfaceKHR>(1);  // Mock a valid surface
                return VK_SUCCESS;
            });

        // Create VulkanInstance using the mock loader
        vulkanInstance = new VulkanInstance(mockWindow, validationLayers, mockLoader);
    }

    void TearDown() override {
        delete vulkanInstance;
    }
};

// Test for successful instance and surface creation
TEST_F(VulkanInstanceTest, CreateInstanceSuccess) {
    // Ensure instance is not null and has a valid handle
    VkInstance* instance = vulkanInstance->getInstance();
    ASSERT_NE(instance, nullptr);
    ASSERT_NE(*instance, VK_NULL_HANDLE);

    // Ensure surface is not null and has a valid handle
    VkSurfaceKHR surface = vulkanInstance->getSurface();
    ASSERT_NE(surface, VK_NULL_HANDLE);
}

// Test for Vulkan instance creation failure
TEST_F(VulkanInstanceTest, CreateInstanceFailure) {
    // Expect vkCreateInstance to fail
    EXPECT_CALL(mockLoader, vkCreateInstance)
        .WillOnce([](const VkInstanceCreateInfo*, const VkAllocationCallbacks*, VkInstance*) {
            return VK_ERROR_INITIALIZATION_FAILED;  // Simulate failure
        });

    EXPECT_THROW({
        delete vulkanInstance;
        vulkanInstance = new VulkanInstance(mockWindow, validationLayers, mockLoader);
    }, std::runtime_error);
}

// Test for surface creation failure
TEST_F(VulkanInstanceTest, CreateSurfaceFailure) {
    // Expect glfwCreateWindowSurface to fail
    EXPECT_CALL(mockLoader, glfwCreateWindowSurface)
        .WillOnce([](VkInstance, GLFWwindow*, const VkAllocationCallbacks*, VkSurfaceKHR*) {
            return VK_ERROR_SURFACE_LOST_KHR;  // Simulate failure
        });

    EXPECT_THROW({
        delete vulkanInstance;
        vulkanInstance = new VulkanInstance(mockWindow, validationLayers, mockLoader);
    }, std::runtime_error);
}
// // Mock versions of Vulkan and GLFW functions
// VkResult mock_vkCreateInstance(const VkInstanceCreateInfo* pCreateInfo, 
//                                const VkAllocationCallbacks* pAllocator, 
//                                VkInstance* pInstance) {
//     *pInstance = reinterpret_cast<VkInstance>(1);  // Mock a valid Vulkan instance
//     return VK_SUCCESS;  // Simulate successful instance creation
// }
//
// VkResult mock_glfwCreateWindowSurface(VkInstance instance, GLFWwindow* window, 
//                                       const VkAllocationCallbacks* allocator, 
//                                       VkSurfaceKHR* surface) {
//     *surface = reinterpret_cast<VkSurfaceKHR>(1);  // Mock a valid surface
//     return VK_SUCCESS;  // Simulate successful surface creation
// }
//
// // Function pointers for Vulkan and GLFW functions
// typedef VkResult (*PFN_vkCreateInstance)(const VkInstanceCreateInfo*, const VkAllocationCallbacks*, VkInstance*);
// typedef VkResult (*PFN_glfwCreateWindowSurface)(VkInstance, GLFWwindow*, const VkAllocationCallbacks*, VkSurfaceKHR*);
//
// PFN_vkCreateInstance originalVkCreateInstance = vkCreateInstance;
// PFN_glfwCreateWindowSurface originalGlfwCreateWindowSurface = glfwCreateWindowSurface;
//
// // Test class
// class VulkanInstanceTest : public ::testing::Test {
// protected:
//     GLFWwindow* mockWindow;
//     ValidationLayers validationLayers;
//     VulkanInstance* vulkanInstance;
//
//     void SetUp() override {
//         // Mock a GLFW window
//         mockWindow = reinterpret_cast<GLFWwindow*>(1);
//
//         // Redirect Vulkan and GLFW function calls to mocks
//         vkCreateInstance = mock_vkCreateInstance;
//         glfwCreateWindowSurface = mock_glfwCreateWindowSurface;
//
//         // Create VulkanInstance (this will call createInstance and createSurface)
//         vulkanInstance = new VulkanInstance(mockWindow, validationLayers);
//     }
//
//     void TearDown() override {
//         delete vulkanInstance;
//
//         // Restore original functions after the test
//         vkCreateInstance = originalVkCreateInstance;
//         glfwCreateWindowSurface = originalGlfwCreateWindowSurface;
//     }
// };
//
// // Test for successful instance and surface creation
// TEST_F(VulkanInstanceTest, CreateInstanceSuccess) {
//     VkInstance* instance = vulkanInstance->getInstance();
//     ASSERT_NE(instance, nullptr);  // Ensure instance is not null
//     ASSERT_NE(*instance, VK_NULL_HANDLE);  // Ensure a valid Vulkan instance was created
//
//     VkSurfaceKHR surface = vulkanInstance->getSurface();
//     ASSERT_NE(surface, VK_NULL_HANDLE);  // Ensure valid surface was created
// }
//
// // Test for Vulkan instance creation failure
// TEST_F(VulkanInstanceTest, CreateInstanceFailure) {
//     // Mock failure of vkCreateInstance
//     vkCreateInstance = [](const VkInstanceCreateInfo*, const VkAllocationCallbacks*, VkInstance*) {
//         return VK_ERROR_INITIALIZATION_FAILED;  // Simulate failure
//     };
//
//     EXPECT_THROW({
//         delete vulkanInstance;  // Clean up the previous instance
//         vulkanInstance = new VulkanInstance(mockWindow, validationLayers);  // This should throw
//     }, std::runtime_error);
//
//     // Restore the original function after the test
//     vkCreateInstance = originalVkCreateInstance;
// }
//
// // Test for surface creation failure
// TEST_F(VulkanInstanceTest, CreateSurfaceFailure) {
//     // Mock failure of glfwCreateWindowSurface
//     glfwCreateWindowSurface = [](VkInstance, GLFWwindow*, const VkAllocationCallbacks*, VkSurfaceKHR*) {
//         return VK_ERROR_SURFACE_LOST_KHR;  // Simulate failure
//     };
//
//     EXPECT_THROW({
//         delete vulkanInstance;  // Clean up the previous instance
//         vulkanInstance = new VulkanInstance(mockWindow, validationLayers);  // This should throw
//     }, std::runtime_error);
//
//     // Restore the original function after the test
//     glfwCreateWindowSurface = originalGlfwCreateWindowSurface;
// }


// Test that the Vulkan surface is created successfully
// TEST_F(VulkanInstanceTest, CreateSurfaceSuccess) {
//     VkSurfaceKHR surface = vulkanInstance->getSurface();
//     ASSERT_NE(surface, VK_NULL_HANDLE);  // Ensure a valid surface was created
// }
//
// // Test that createInstance throws an error when Vulkan instance creation fails
// TEST_F(VulkanInstanceTest, CreateInstanceFailure) {
//     // Override the vkCreateInstanceFunc to simulate failure
//     vkCreateInstanceFunc = mock_vkCreateInstanceFailure;
//
//     // Test that the constructor throws an exception when instance creation fails
//     EXPECT_THROW({
//         VulkanInstance invalidInstance(mockWindow, validationLayers);
//     }, std::runtime_error);
//
//     // Restore original mock after the test
//     vkCreateInstanceFunc = mock_vkCreateInstance;
// }
//
// // Test that createSurface throws an error when GLFW surface creation fails
// TEST_F(VulkanInstanceTest, CreateSurfaceFailure) {
//     // Override the glfwCreateWindowSurfaceFunc to simulate failure
//     glfwCreateWindowSurfaceFunc = mock_glfwCreateWindowSurfaceFailure;
//
//     // Test that the constructor throws an exception when surface creation fails
//     EXPECT_THROW({
//         VulkanInstance invalidInstance(mockWindow, validationLayers);
//     }, std::runtime_error);
//
//     // Restore original mock after the test
//     glfwCreateWindowSurfaceFunc = mock_glfwCreateWindowSurface;
// }

// ================================================================================
// ================================================================================
// eof
