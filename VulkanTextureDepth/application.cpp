// ================================================================================
// ================================================================================
// - File:    application.cpp
// - Purpose: Describe the file purpose here
//
// Source Metadata
// - Author:  Jonathan A. Webb
// - Date:    June 19, 2024
// - Version: 1.0
// - Copyright: Copyright 2022, Jon Webb Inc.
// ================================================================================
// ================================================================================
// Include modules here
#include "include/application.hpp"
#include "include/constants.hpp"

#include <vector>
#include <iostream>
#include <glm/glm.hpp>            // Core GLM functionality
#include <glm/gtc/matrix_transform.hpp>  // For glm::rotate, glm::lookAt, glm::perspective
#include <chrono>
// ================================================================================
// ================================================================================

VulkanInstance::VulkanInstance(GLFWwindow* window, ValidationLayers& validationLayers)
    : windowInstance(window), validationLayers(validationLayers) {

    createInstance();
    createSurface();
}
// --------------------------------------------------------------------------------

VulkanInstance::~VulkanInstance() {
    {
        std::lock_guard<std::mutex> lockSurface(surfaceMutex);
        if (surface != VK_NULL_HANDLE) {
            vkDestroySurfaceKHR(instance, surface, nullptr);
            surface = VK_NULL_HANDLE;
        }
    }

    {
        std::lock_guard<std::mutex> lockInstance(instanceMutex);
        if (instance != VK_NULL_HANDLE) {
            validationLayers.cleanup(instance);
            vkDestroyInstance(instance, nullptr);
            instance = VK_NULL_HANDLE;
        }
    }
}
// --------------------------------------------------------------------------------


VkInstance* VulkanInstance::getInstance() {
    std::lock_guard<std::mutex> lock(instanceMutex);
    return &instance;
}
// --------------------------------------------------------------------------------

VkSurfaceKHR VulkanInstance::getSurface() {
    std::lock_guard<std::mutex> lock(surfaceMutex);
    return surface;
}
// ================================================================================

void VulkanInstance::createInstance() {
    std::lock_guard<std::mutex> lock(instanceMutex);  // Protect creation of instance

    if (validationLayers.isEnabled() && !validationLayers.checkValidationLayerSupport()) {
        throw std::runtime_error("Validation layers requested, but not available!");
    }

    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "VulkanTriangle";
    appInfo.applicationVersion = VK_MAKE_VERSION(0, 1, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_3;

    uint32_t extensionCount = 0;
    const char** extensions = glfwGetRequiredInstanceExtensions(&extensionCount);
    std::vector<const char*> extensionVector(extensions, extensions + extensionCount);

    if (validationLayers.isEnabled()) {
        std::vector<const char*> validationLayerExtensions = validationLayers.getRequiredExtensions();
        extensionVector.insert(extensionVector.end(), validationLayerExtensions.begin(), validationLayerExtensions.end());
    }

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensionVector.size());
    createInfo.ppEnabledExtensionNames = extensionVector.data();

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
    if (validationLayers.isEnabled()) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.getValidationLayers().size());
        createInfo.ppEnabledLayerNames = validationLayers.getValidationLayers().data();
        validationLayers.populateDebugMessengerCreateInfo(debugCreateInfo);
        createInfo.pNext = &debugCreateInfo;
    } else {
        createInfo.enabledLayerCount = 0;
        createInfo.ppEnabledLayerNames = nullptr;
        createInfo.pNext = nullptr;
    }

    if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create Vulkan instance!");
    }

    if (validationLayers.isEnabled()) {
        validationLayers.setupDebugMessenger(instance);
    }
}
// --------------------------------------------------------------------------------

void VulkanInstance::createSurface() {
    std::lock_guard<std::mutex> lock(surfaceMutex);
    if (glfwCreateWindowSurface(instance, windowInstance, nullptr, &surface) != VK_SUCCESS)
        throw std::runtime_error("Failed to create window surface\n");
}
// ================================================================================
// ================================================================================


VulkanApplication::VulkanApplication(GLFWwindow* window, 
                                     const std::vector<Vertex>& vertices,
                                     const std::vector<uint16_t>& indices)
    : windowInstance(std::move(window)),
      vertices(vertices),
      indices(indices){
    // Instantiate related classes
    glfwSetWindowUserPointer(windowInstance, this);

    validationLayers = std::make_unique<ValidationLayers>();
    vulkanInstanceCreator = std::make_unique<VulkanInstance>(this->windowInstance, 
                                                             *validationLayers.get());
    vulkanPhysicalDevice = std::make_unique<VulkanPhysicalDevice>(*this->vulkanInstanceCreator->getInstance(),
                                                                  this->vulkanInstanceCreator->getSurface());
    vulkanLogicalDevice = std::make_unique<VulkanLogicalDevice>(vulkanPhysicalDevice->getDevice(),
                                                                validationLayers->getValidationLayers(),
                                                                vulkanInstanceCreator->getSurface(),
                                                                deviceExtensions);
    allocatorManager = std::make_unique<AllocatorManager>(
        vulkanPhysicalDevice->getDevice(),
        vulkanLogicalDevice->getDevice(),
        *vulkanInstanceCreator->getInstance());

    swapChain = std::make_unique<SwapChain>(vulkanLogicalDevice->getDevice(),
                                            vulkanInstanceCreator->getSurface(),
                                            vulkanPhysicalDevice->getDevice(),
                                            this->windowInstance);
    commandBufferManager = std::make_unique<CommandBufferManager>(vulkanLogicalDevice->getDevice(),
                                                                  indices,
                                                                  vulkanPhysicalDevice->getDevice(),
                                                                  vulkanInstanceCreator->getSurface());
    samplerManager = std::make_unique<SamplerManager>(
            vulkanLogicalDevice->getDevice(),
            vulkanPhysicalDevice->getDevice()
    );
    samplerManager->createSampler("default");
    textureManager = std::make_unique<TextureManager>(
        *allocatorManager,                              // Dereference unique_ptr
        vulkanLogicalDevice->getDevice(),
        vulkanPhysicalDevice->getDevice(),
        *commandBufferManager,                          // Dereference unique_ptr
        vulkanLogicalDevice->getGraphicsQueue(),
        "../../../data/texture.jpg",
        *samplerManager
    );
    bufferManager = std::make_unique<BufferManager>(vertices,
                                                    indices,
                                                    *allocatorManager,
                                                    *commandBufferManager.get(),
                                                    vulkanLogicalDevice->getGraphicsQueue());
    descriptorManager = std::make_unique<DescriptorManager>(vulkanLogicalDevice->getDevice());
    descriptorManager->createDescriptorSets(bufferManager->getUniformBuffers(),
                                            textureManager->getTextureImageView(),
                                            samplerManager->getSampler("default"));
    graphicsPipeline = std::make_unique<GraphicsPipeline>(vulkanLogicalDevice->getDevice(),
                                                          *swapChain.get(),
                                                          *commandBufferManager.get(),
                                                          *bufferManager.get(),
                                                          *descriptorManager.get(),
                                                          indices,
                                                          vulkanPhysicalDevice->getDevice(),
                                                          std::string("../../shaders/shader.vert.spv"),
                                                          std::string("../../shaders/shader.frag.spv"));
    graphicsPipeline->createFrameBuffers(swapChain->getSwapChainImageViews(), 
                                         swapChain->getSwapChainExtent());
    graphicsQueue = this->vulkanLogicalDevice->getGraphicsQueue();
    presentQueue = this->vulkanLogicalDevice->getPresentQueue();
}
// -------------------------------------------------------------------------------- 

VulkanApplication::~VulkanApplication() {
    destroyResources();
}
// --------------------------------------------------------------------------------

void VulkanApplication::scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    VulkanApplication* app = reinterpret_cast<VulkanApplication*>(glfwGetWindowUserPointer(window));
    if (app) {
        app->zoomLevel -= yoffset * 0.1f; // Adjust zoom sensitivity
        app->zoomLevel = glm::clamp(app->zoomLevel, 0.1f, 5.0f); // Clamp zoom level to reasonable limits
    }
}
// --------------------------------------------------------------------------------

void VulkanApplication::run() {
    glfwSetScrollCallback(windowInstance, scrollCallback);
    while (!glfwWindowShouldClose(windowInstance)) {
        glfwPollEvents();
        drawFrame();

        if (framebufferResized) {
            recreateSwapChain();
            framebufferResized = false;
        }
    }
    vkDeviceWaitIdle(vulkanLogicalDevice->getDevice());
}
// ================================================================================

void VulkanApplication::destroyResources() {

    commandBufferManager.reset();
    samplerManager.reset();
    textureManager.reset();
    bufferManager.reset();
    descriptorManager.reset();
    graphicsPipeline.reset();
    allocatorManager.reset();
    swapChain.reset();

    // Destroy Vulkan logical device first
    vulkanLogicalDevice.reset();

    // Destroy other Vulkan resources
    vulkanPhysicalDevice.reset();
    vulkanInstanceCreator.reset();
}
// --------------------------------------------------------------------------------

void VulkanApplication::drawFrame() {
    VkDevice device = vulkanLogicalDevice->getDevice();
    uint32_t frameIndex = currentFrame;

    // Wait for the frame to be finished
    commandBufferManager->waitForFences(frameIndex);
    commandBufferManager->resetFences(frameIndex);
    uint32_t imageIndex;
    VkResult result = vkAcquireNextImageKHR(device, swapChain->getSwapChain(), UINT64_MAX, 
                                            commandBufferManager->getImageAvailableSemaphore(frameIndex), 
                                            VK_NULL_HANDLE, &imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        recreateSwapChain(); // Recreate swap chain if it's out of date
        return;
    } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        throw std::runtime_error("failed to acquire swap chain image!");
    }

    // Update the uniform buffer with the current image/frame
    updateUniformBuffer(frameIndex);

    VkCommandBuffer cmdBuffer = commandBufferManager->getCommandBuffer(frameIndex);

    vkResetCommandBuffer(cmdBuffer, 0);

    graphicsPipeline->recordCommandBuffer(frameIndex, imageIndex);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = {commandBufferManager->getImageAvailableSemaphore(frameIndex)};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &cmdBuffer;

    VkSemaphore signalSemaphores[] = {commandBufferManager->getRenderFinishedSemaphore(frameIndex)};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, commandBufferManager->getInFlightFence(frameIndex)) != VK_SUCCESS) {
        throw std::runtime_error("failed to submit draw command buffer!");
    }

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = {swapChain->getSwapChain()};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;

    result = vkQueuePresentKHR(presentQueue, &presentInfo);
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebufferResized) {
        framebufferResized = false;
        recreateSwapChain();  // Recreate swap chain if it's out of date or suboptimal
    } else if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to present swap chain image!");
    }

    currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}
// --------------------------------------------------------------------------------

void VulkanApplication::framebufferResizeCallback(GLFWwindow* window, int width, int height) {
    auto app = reinterpret_cast<VulkanApplication*>(glfwGetWindowUserPointer(window));
    if (app) {
        app->setFramebufferResized(true);
    }
}
// --------------------------------------------------------------------------------

void VulkanApplication::recreateSwapChain() {
    // If the window is minimized, pause execution until the window is resized again
    int width = 0, height = 0;
    GLFWwindow* glfwWindow = static_cast<GLFWwindow*>(windowInstance);
    glfwGetFramebufferSize(glfwWindow, &width, &height);

    while (width == 0 || height == 0) {
        glfwGetFramebufferSize(glfwWindow, &width, &height);
        glfwWaitEvents();
    }

    // Wait for the device to be idle before starting swap chain recreation
    vkDeviceWaitIdle(vulkanLogicalDevice->getDevice());

    // Clean up the old swap chain-related resources
    graphicsPipeline->destroyFramebuffers();  // Destroy the old framebuffers
    swapChain->cleanupSwapChain();            // Clean up the old swap chain (destroy image views, etc.)

    // Recreate the swap chain and dependent resources
    swapChain->recreateSwapChain();           // Create a new swap chain and its image views

    // Recreate the framebuffers using the new swap chain image views
    graphicsPipeline->createFrameBuffers(swapChain->getSwapChainImageViews(), swapChain->getSwapChainExtent());

    // Free and recreate command buffers frame by frame
    VkCommandPool commandPool = commandBufferManager->getCommandPool();
    for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        VkCommandBuffer cmdBuffer = commandBufferManager->getCommandBuffer(i);
        vkFreeCommandBuffers(vulkanLogicalDevice->getDevice(), commandPool, 1, &cmdBuffer); // Free single command buffer
    }

    // Recreate the command buffers
    commandBufferManager->createCommandBuffers();
}
// --------------------------------------------------------------------------------

void VulkanApplication::updateUniformBuffer(uint32_t currentImage) {
    static auto startTime = std::chrono::high_resolution_clock::now();
    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

    UniformBufferObject ubo{};
    ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    float fov = glm::radians(45.0f) / zoomLevel; // Adjust FOV with zoom level
    ubo.proj = glm::perspective(fov, swapChain->getSwapChainExtent().width / (float)swapChain->getSwapChainExtent().height, 0.1f, 10.0f);
    ubo.proj[1][1] *= -1; // Invert Y-axis for Vulkan

    memcpy(bufferManager->getUniformBuffersMapped()[currentImage], &ubo, sizeof(ubo));
}
// ================================================================================
// ================================================================================
// eof
