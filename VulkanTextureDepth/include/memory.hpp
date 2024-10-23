// ================================================================================
// ================================================================================
// - File:    memory.hpp
// - Purpose: Describe the file purpose here
//
// Source Metadata
// - Author:  Jonathan A. Webb
// - Date:    August 04, 2024
// - Version: 1.0
// - Copyright: Copyright 2022, Jon Webb Inc.
// ================================================================================
// ================================================================================
// Include modules here

#ifndef memory_HPP
#define memory_HPP
// ================================================================================
// ================================================================================

#include <vk_mem_alloc.h>
#include <vulkan/vulkan.h>
#include <iostream>
// ================================================================================
// ================================================================================

/**
 * @class AllocatorManager
 * @brief Manages Vulkan buffers and memory allocations using the Vulkan Memory Allocator (VMA).
 */
class AllocatorManager {
public:
    /**
     * @brief Constructs the AllocatorManager and initializes the VMA allocator.
     * @param physicalDevice The Vulkan physical device.
     * @param device The Vulkan logical device.
     * @param instance The Vulkan instance.
     * @throws std::runtime_error If the VMA allocator cannot be created.
     */
    AllocatorManager(VkPhysicalDevice physicalDevice, VkDevice device, VkInstance instance);
// --------------------------------------------------------------------------------

    /**
     * @brief Destructor that cleans up the VMA allocator.
     */
    ~AllocatorManager();
// --------------------------------------------------------------------------------

    /**
     * @brief Creates a Vulkan buffer and allocates memory for it using VMA.
     * @param size The size of the buffer in bytes.
     * @param usage The usage flags for the buffer (e.g., transfer source, vertex buffer).
     * @param memoryUsage The memory usage type (e.g., VMA_MEMORY_USAGE_GPU_ONLY).
     * @param buffer A reference to the created Vulkan buffer.
     * @param allocation A reference to the VMA allocation for the buffer's memory.
     * @throws std::runtime_error If buffer creation or memory allocation fails.
     */
    void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VmaMemoryUsage memoryUsage,
                      VkBuffer& buffer, VmaAllocation& allocation);
// --------------------------------------------------------------------------------

    /**
     * @brief Maps the memory associated with a VMA allocation to a CPU-accessible pointer.
     * @param allocation The VMA allocation to map.
     * @param data A pointer to the location where the mapped memory address will be stored.
     * @throws std::runtime_error If memory mapping fails.
     */
    void mapMemory(VmaAllocation allocation, void** data);
// --------------------------------------------------------------------------------

    /**
     * @brief Unmaps the previously mapped memory for a VMA allocation.
     * @param allocation The VMA allocation to unmap.
     */
    void unmapMemory(VmaAllocation allocation);
// --------------------------------------------------------------------------------

    /**
     * @brief Destroys a Vulkan buffer and frees its associated memory allocation.
     * @param buffer The Vulkan buffer to destroy.
     * @param allocation The VMA allocation to free.
     */
    void destroyBuffer(VkBuffer buffer, VmaAllocation allocation);
// --------------------------------------------------------------------------------

    /**
     * @brief Copies data from one buffer to another.
     * @param srcBuffer The source buffer.
     * @param dstBuffer The destination buffer.
     * @param size The size of the data to copy.
     * @param graphicsQueue The graphics queue to submit the copy command.
     * @param commandPool The command pool to allocate command buffers from.
     * @throws std::runtime_error If command buffer allocation or submission fails.
     */
    void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size, 
                    VkQueue graphicsQueue, VkCommandPool commandPool);
// --------------------------------------------------------------------------------

    /**
     * @brief Returns the VMA allocator instance.
     * @return The VMA allocator used by this manager.
     */
    VmaAllocator getAllocator() const;
// ================================================================================
private:
    VkDevice device;
    VmaAllocator allocator;
};
// ================================================================================
// ================================================================================

#endif /* file_name_HPP */
// ================================================================================
// ================================================================================
// eof
