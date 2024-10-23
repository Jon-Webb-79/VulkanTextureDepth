// ================================================================================
// ================================================================================
// - File:    validation_layers.hpp
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

#ifndef validation_layers_HPP
#define validation_layers_HPP

#include <vector>
#include <memory>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
// ================================================================================
// ================================================================================

/**
 * @brief Creates a Vulkan debug utils messenger.
 * 
 * This function creates a debug messenger for Vulkan that will capture and log debug messages from the Vulkan API.
 * 
 * @param instance The Vulkan instance to associate with the debug messenger.
 * @param pCreateInfo A pointer to a VkDebugUtilsMessengerCreateInfoEXT structure containing details about the debug messenger to be created.
 * @param pAllocator An optional pointer to a VkAllocationCallbacks structure that controls host memory allocation. If null, the default allocator is used.
 * @param pDebugMessenger A pointer to a VkDebugUtilsMessengerEXT handle in which the created debug messenger is returned.
 * 
 * @return VK_SUCCESS if the debug messenger was successfully created, or an error code otherwise.
 */
VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, 
                                      const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, 
                                      const VkAllocationCallbacks* pAllocator, 
                                      VkDebugUtilsMessengerEXT* pDebugMessenger);
// --------------------------------------------------------------------------------

/**
 * @brief Destroys a Vulkan debug utils messenger.
 * 
 * This function destroys a previously created debug messenger, freeing any associated resources.
 * 
 * @param instance The Vulkan instance associated with the debug messenger.
 * @param debugMessenger The debug messenger to destroy.
 * @param pAllocator An optional pointer to a VkAllocationCallbacks structure that controls host memory allocation. If null, the default allocator is used.
 */
void DestroyDebugUtilsMessengerEXT(VkInstance instance, 
                                   VkDebugUtilsMessengerEXT debugMessenger, 
                                   const VkAllocationCallbacks* pAllocator);
// ================================================================================
// ================================================================================
/**
 * @class ValidationLayers
 * @brief Handles setup and management of Vulkan validation layers.
 */
class ValidationLayers {
public:
    /**
     * @brief Constructs the ValidationLayers object.
     */
    ValidationLayers();
// --------------------------------------------------------------------------------

    /**
     * @brief Destroys the ValidationLayers object.
     */
    ~ValidationLayers();
// --------------------------------------------------------------------------------

    /**
     * @brief Checks if validation layers are enabled.
     * @return True if validation layers are enabled, false otherwise.
     */
    bool isEnabled() const;
// --------------------------------------------------------------------------------

    /**
     * @brief Retrieves the required Vulkan instance extensions.
     * @return A vector containing the names of the required extensions.
     */
    std::vector<const char*> getRequiredExtensions() const;
// --------------------------------------------------------------------------------

    /**
     * @brief Sets up the debug messenger for Vulkan validation layers.
     * @param instance The Vulkan instance.
     */
    void setupDebugMessenger(VkInstance instance);
// --------------------------------------------------------------------------------

    /**
     * @brief Cleans up the debug messenger.
     * @param instance The Vulkan instance.
     */
    void cleanup(VkInstance instance);
// --------------------------------------------------------------------------------

    /**
     * @brief Retrieves the debug messenger handle.
     * @return The Vulkan debug messenger handle.
     */
    VkDebugUtilsMessengerEXT getDebugMessenger() const;
// --------------------------------------------------------------------------------

    /**
     * @brief Checks if the requested validation layers are available.
     * @return True if all requested validation layers are available, false otherwise.
     */
    bool checkValidationLayerSupport();
// --------------------------------------------------------------------------------

    /**
     * @brief Retrieves the validation layers.
     * @return A vector containing the names of the validation layers.
     */
    const std::vector<const char*>& getValidationLayers() const;
// --------------------------------------------------------------------------------
    /**
     * @brief Populates the VkDebugUtilsMessengerCreateInfoEXT structure.
     * @param createInfo The create info structure to populate.
     */
    void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
// ================================================================================ 
private:

//    GlfwWindow& window;

    /**
     * @brief Callback function for the Vulkan debug messenger.
     * @param messageSeverity The severity of the message.
     * @param messageType The type of the message.
     * @param pCallbackData Additional data about the message.
     * @param pUserData User data provided to the callback function.
     * @return VK_FALSE to indicate that the application should not abort.
     */
    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData
    );
// --------------------------------------------------------------------------------

    VkDebugUtilsMessengerEXT debugMessenger; ///< The Vulkan debug messenger handle.
    const std::vector<const char*> validationLayers = {
        "VK_LAYER_KHRONOS_validation"
    }; ///< The list of requested validation layers.
// --------------------------------------------------------------------------------

#ifdef NDEBUG
    const bool enableValidationLayers = false;
#else
    const bool enableValidationLayers = true;
#endif
};
// ================================================================================
// ================================================================================

#endif /* file_name_HPP */
// ================================================================================
// ================================================================================
// eof
