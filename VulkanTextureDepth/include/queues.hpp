// ================================================================================
// ================================================================================
// - File:    queues.hpp
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

#ifndef queues_HPP
#define queues_HPP

#include <vulkan/vulkan.h>
#include <optional>
// ================================================================================
// ================================================================================ 

/**
 * @struct QueueFamilyIndices
 * @brief Represents the indices of queue families for a Vulkan physical device.
 *
 * This struct is used to store the indices of the queue families that support
 * graphics and presentation operations for a Vulkan physical device.
 */
struct QueueFamilyIndices {
    /**
     * @brief Optional index for the graphics queue family.
     *
     * This member stores the index of the queue family that supports graphics operations.
     */
    std::optional<uint32_t> graphicsFamily;
    /**
     * @brief Optional index for the presentation queue family.
     *
     * This member stores the index of the queue family that supports presentation operations.
     */
    std::optional<uint32_t> presentFamily;
// --------------------------------------------------------------------------------
    
    /**
     * @brief Checks if both graphics and presentation queue families are found.
     *
     * @return True if both graphicsFamily and presentFamily have values, false otherwise.
     */
    bool isComplete() const {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
};
// ================================================================================
// ================================================================================

/**
 * @class QueueFamily
 * @brief Provides utility functions for finding queue families in a Vulkan physical device.
 *
 * This class contains static methods that help in finding the indices of queue families
 * that support specific operations such as graphics and presentation.
 */
class QueueFamily {
public:
    /**
     * @brief Finds the queue families that support graphics and presentation operations.
     *
     * This method queries the given Vulkan physical device to find the queue families
     * that support graphics and presentation operations. It returns a QueueFamilyIndices
     * struct containing the indices of the found queue families.
     *
     * @param device The Vulkan physical device to query.
     * @param surface The Vulkan surface for presentation support.
     * @return A QueueFamilyIndices struct containing the indices of the graphics and presentation queue families.
     */
    static QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface);
};
// ================================================================================
// ================================================================================

#endif /* queues_HPP */
// ================================================================================
// ================================================================================
// eof
