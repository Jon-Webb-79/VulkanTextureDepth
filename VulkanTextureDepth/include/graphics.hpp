// ================================================================================
// ================================================================================
// - File:    graphics.hpp
// - Purpose: Describe the file purpose here
//
// Source Metadata
// - Author:  Jonathan A. Webb
// - Date:    August 30, 2024
// - Version: 1.0
// - Copyright: Copyright 2022, Jon Webb Inc.
// ================================================================================
// ================================================================================
// Include modules here

#ifndef graphics_HPP
#define graphics_HPP

#include <glm/glm.hpp>
#include <vulkan/vulkan.h>
#include <vector>
#include <array>
#include <string>
#include <cstddef>
#include <unordered_map>

#include "memory.hpp"
#include "devices.hpp"
#include <vk_mem_alloc.h>
// ================================================================================
// ================================================================================ 

static constexpr uint32_t MAX_FRAMES_IN_FLIGHT = 2;
// ================================================================================
// ================================================================================ 


/**
 * @brief Represents a vertex with position and color attributes.
 *
 * This struct defines a vertex with a 2D position and a 3D color. It also provides
 * static methods to describe how these vertex attributes are laid out in memory
 * for Vulkan's vertex input system. 
 */
struct Vertex {
    glm::vec2 pos;
    glm::vec3 color;
    glm::vec2 texCoord;
// --------------------------------------------------------------------------------

    /**
     * @brief Returns the binding description for the vertex input.
     *
     * This function specifies how the vertex data is organized in the vertex buffer.
     * It provides the binding index, the byte stride between consecutive vertex data,
     * and the rate at which the input should advance.
     * 
     * @return A VkVertexInputBindingDescription struct that describes the input binding.
     */
    static VkVertexInputBindingDescription getBindingDescription() {
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(Vertex);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return bindingDescription;
    }
// --------------------------------------------------------------------------------

    /**
     * @brief Returns the attribute descriptions for the vertex input.
     *
     * This function describes the vertex attributes (position and color) and their
     * layout in memory. It specifies the format of each attribute and the byte offset
     * from the start of the vertex structure.
     * 
     * @return A std::array of VkVertexInputAttributeDescription structs that describe the vertex attributes.
     */
    static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions() {
        std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions{};

        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(Vertex, pos);

        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(Vertex, color);

        attributeDescriptions[2].binding = 0;
        attributeDescriptions[2].location = 2;
        attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[2].offset = offsetof(Vertex, texCoord);

        return attributeDescriptions;
    }
    // static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions() {
    //     std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};
    //
    //     attributeDescriptions[0].binding = 0;
    //     attributeDescriptions[0].location = 0;
    //     attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
    //     attributeDescriptions[0].offset = offsetof(Vertex, pos);
    //
    //     attributeDescriptions[1].binding = 0;
    //     attributeDescriptions[1].location = 1;
    //     attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    //     attributeDescriptions[1].offset = offsetof(Vertex, color);
    //
    //     return attributeDescriptions;
    // }
};
// ================================================================================
// ================================================================================


struct UniformBufferObject {
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 proj;
};
// ================================================================================
// ================================================================================


/**
 * @class CommandBufferManager
 * @brief Manages the creation, allocation, and synchronization of command buffers, fences, and semaphores for Vulkan.
 *
 * This class encapsulates the command buffer management in Vulkan, providing methods for creating command pools,
 * allocating command buffers, and managing synchronization primitives like fences and semaphores.
 */
class CommandBufferManager {
public:
    
    /**
     * @brief Constructor for CommandBufferManager.
     *
     * @param device The Vulkan device handle used for command buffer and resource creation.
     * @param indices The vector of indices used for managing the command buffers.
     * @param physicalDevice The Vulkan physical device used to create the command pool.
     * @param surface The Vulkan surface handle used for surface-related operations.
     */
    CommandBufferManager(VkDevice device,
                         const std::vector<uint16_t>& indices,
                         VkPhysicalDevice physicalDevice,
                         VkSurfaceKHR surface);
// --------------------------------------------------------------------------------
    
    /**
     * @brief Destructor for CommandBufferManager. Cleans up resources like command buffers, semaphores, and fences.
     */
    ~CommandBufferManager();
// --------------------------------------------------------------------------------
    
    /**
     * @brief Waits for a specific frame's fences to be signaled before proceeding.
     *
     * @param frameIndex The index of the frame whose fence should be waited for.
     */
    void waitForFences(uint32_t frameIndex) const;
// --------------------------------------------------------------------------------

    /**
     * @brief Resets the fences for a specific frame, allowing it to be reused.
     *
     * @param frameIndex The index of the frame whose fence should be reset.
     */
    void resetFences(uint32_t) const;
// --------------------------------------------------------------------------------

    /**
     * @brief Retrieves the Vulkan command pool associated with this manager.
     *
     * @return The Vulkan command pool.
     */
    const VkCommandPool& getCommandPool() const;
// --------------------------------------------------------------------------------

    /**
     * @brief Retrieves the vector of command buffers managed by this class.
     *
     * @return A reference to the vector of Vulkan command buffers.
     */
    const std::vector<VkCommandBuffer>& getCommandBuffers() const;
// --------------------------------------------------------------------------------
    
    /**
     * @brief Retrieves a specific command buffer for a given frame index.
     *
     * @param frameIndex The index of the frame for which the command buffer is retrieved.
     * @return The Vulkan command buffer for the specified frame.
     */
    const VkCommandBuffer& getCommandBuffer(uint32_t frameIndex) const;
// --------------------------------------------------------------------------------

    /**
     * @brief Retrieves the image available semaphore for a given frame index.
     *
     * @param frameIndex The index of the frame for which the image available semaphore is retrieved.
     * @return The Vulkan semaphore for signaling image availability.
     */
    const VkSemaphore& getImageAvailableSemaphore(uint32_t frameIndex) const;
// --------------------------------------------------------------------------------

    /**
     * @brief Retrieves the render finished semaphore for a given frame index.
     *
     * @param frameIndex The index of the frame for which the render finished semaphore is retrieved.
     * @return The Vulkan semaphore for signaling when rendering is finished.
     */
    const VkSemaphore& getRenderFinishedSemaphore(uint32_t frameIndex) const;
// --------------------------------------------------------------------------------

    /**
     * @brief Retrieves the in-flight fence for a given frame index.
     *
     * @param frameIndex The index of the frame for which the in-flight fence is retrieved.
     * @return The Vulkan fence for synchronization of frame rendering.
     */
    const VkFence& getInFlightFence(uint32_t frameIndex) const;
// --------------------------------------------------------------------------------

    /**
     * @brief Allocates and creates the command buffers for rendering.
     */
    void createCommandBuffers();
// ================================================================================
private:
    // Attributes passed to constructor
    VkDevice device;                      /**< The Vulkan device handle. */
    VkExtent2D swapChainExtent;           /**< The extent of the swap chain for rendering. */
    std::vector<uint16_t> indices;        /**< Vector holding index data for command buffers. */

    VkCommandPool commandPool = VK_NULL_HANDLE; /**< The Vulkan command pool used to allocate command buffers. */
    std::vector<VkCommandBuffer> commandBuffers; /**< The list of Vulkan command buffers. */
    std::vector<VkSemaphore> imageAvailableSemaphores; /**< Semaphores used to signal when images are available. */
    std::vector<VkSemaphore> renderFinishedSemaphores; /**< Semaphores used to signal when rendering is finished. */
    std::vector<VkFence> inFlightFences; /**< Fences used for synchronizing frame rendering. */ 
// --------------------------------------------------------------------------------

    /**
     * @brief Creates a command pool for allocating command buffers.
     *
     * @param physicalDevice The Vulkan physical device used to create the command pool.
     * @param surface The Vulkan surface handle used for surface-related operations.
     */
    void createCommandPool(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);
// --------------------------------------------------------------------------------

    /**
     * @brief Creates synchronization objects like semaphores and fences for rendering.
     */
    void createSyncObjects();
};
// ================================================================================
// ================================================================================

/**
 * @class SamplerManager
 * @brief Manages the creation, retrieval, and destruction of Vulkan texture samplers.
 *
 * The SamplerManager class is responsible for managing Vulkan samplers. 
 * It allows for the creation of reusable samplers based on unique keys, 
 * making it efficient to share samplers across multiple textures that have similar sampling properties.
 */
class SamplerManager {
public:

    /**
     * @brief Constructs a SamplerManager for managing Vulkan texture samplers.
     *
     * Initializes the SamplerManager with the Vulkan device and physical device handles needed
     * for creating samplers.
     * 
     * @param device The Vulkan logical device handle used to create and manage samplers.
     * @param physicalDevice The Vulkan physical device handle used for querying properties 
     *        such as supported anisotropy levels.
     */
    SamplerManager(VkDevice device, VkPhysicalDevice physicalDevice);
// --------------------------------------------------------------------------------

    /**
     * @brief Destructor for the SamplerManager class.
     *
     * Cleans up all Vulkan samplers created and managed by the SamplerManager.
     * Each sampler in the manager is destroyed upon the SamplerManager's destruction.
     */
    ~SamplerManager();
// --------------------------------------------------------------------------------
    /**
     * @brief Retrieves an existing sampler by key.
     *
     * Looks up the sampler associated with the specified key. If the sampler does not exist,
     * it will throw an exception.
     * 
     * @param samplerKey A unique key identifying the desired sampler.
     * @return The Vulkan sampler associated with the given key.
     * @throws std::out_of_range if the sampler associated with the key does not exist.
     */
    VkSampler getSampler(const std::string& samplerKey) const;
// --------------------------------------------------------------------------------
    /**
     * @brief Creates and stores a sampler with the specified key.
     *
     * Creates a new Vulkan sampler with commonly used sampling parameters and associates it
     * with the specified key for future retrieval. The created sampler is stored in the 
     * `samplers` map and can be reused across multiple textures.
     * 
     * @param samplerKey A unique key identifying the sampler for future retrieval.
     * @return The Vulkan sampler created and stored with the specified key.
     * @throws std::runtime_error if Vulkan fails to create the sampler.
     */
    VkSampler createSampler(const std::string& samplerKey);
// ================================================================================
private:

    VkDevice device; /**< The Vulkan logical device handle used for sampler creation. */ 
    VkPhysicalDevice physicalDevice; /**< The Vulkan physical device handle for querying properties. */ 
    std::unordered_map<std::string, VkSampler> samplers;  /**< Map of samplers keyed by unique strings for reuse. */

    mutable std::mutex samplerMutex; /**< Mutex to synchronize access to the `samplers` map. */ 
};
// ================================================================================
// ================================================================================ 

/**
 * @class TextureManager
 * @brief Manages the creation, loading, and transition of textures in Vulkan.
 *
 * The TextureManager class handles the creation of texture images, image layout transitions,
 * and buffer-to-image copies for Vulkan. It leverages the AllocatorManager to manage Vulkan memory 
 * allocations and CommandBufferManager for handling command buffers during the transition and copy 
 * operations. 
 */
class TextureManager {
public:
    /**
     * @brief Constructs a TextureManager to manage Vulkan textures and their resources.
     *
     * Initializes the TextureManager with necessary Vulkan objects and parameters to manage texture images.
     * The texture image is loaded from the specified file path, and the image view is created for shader access.
     * The TextureManager also obtains a sampler from the SamplerManager using the specified sampler key.
     * 
     * @param allocatorManager Reference to an AllocatorManager responsible for managing Vulkan memory.
     * @param device The Vulkan logical device handle used for memory allocations and operations.
     * @param physicalDevice The Vulkan physical device handle used to query memory properties.
     * @param commandBufferManager Reference to the CommandBufferManager responsible for command buffer allocation.
     * @param graphicsQueue The Vulkan queue for submitting graphics commands.
     * @param imagePath Path to the texture image file to be loaded and used as a texture.
     * @param samplerManager Reference to a SamplerManager that manages reusable Vulkan samplers.
     * @param samplerKey The key used to identify the desired sampler from the SamplerManager. 
     *        If no key is specified, the "default" sampler is used.
     * 
     * @throws std::invalid_argument if the imagePath is empty.
     * @throws std::runtime_error if any Vulkan resource creation fails, such as loading the texture image,
     *         creating the Vulkan image or image view, or retrieving the sampler.
     */
    TextureManager(AllocatorManager& allocatorManager,
                   VkDevice device,
                   VkPhysicalDevice physicalDevice,
                   CommandBufferManager& commandBufferManager,
                   VkQueue graphicsQueue,
                   const std::string imagePath,
                   SamplerManager& samplerManager,
                   const std::string& samplerKey = "default");
// --------------------------------------------------------------------------------
    
    /**
     * @brief Destructor for the TextureManager class.
     *
     * Cleans up Vulkan resources related to the texture image, such as freeing the texture memory 
     * and destroying the texture image.
     */
    ~TextureManager();
// --------------------------------------------------------------------------------

    /**
     * @brief Retrieves the texture image view.
     *
     * Provides access to the texture image view used by shaders for sampling the texture.
     *
     * @return The Vulkan image view for the texture.
     */
    VkImageView getTextureImageView() const { return textureImageView; }
// --------------------------------------------------------------------------------

    /**
     * @brief Reloads the texture image from a new file path.
     *
     * This method safely releases the resources associated with the current texture, including the
     * image, image view, and sampler. It then updates the texture path to the specified new image
     * path and recreates the texture resources using the new image. The reloading process involves
     * creating a new Vulkan image, transitioning its layout, and copying buffer data for the new texture.
     *
     * @param newImagePath The file path to the new texture image to be loaded and used as the texture.
     *
     * @throws std::runtime_error if any step in the texture creation process fails, such as image loading,
     *         Vulkan resource allocation, or image transition.
     *
     * @note This function can be used for dynamic texture reloading at runtime. Make sure to call this
     *       method only when the texture resources are not actively in use in the graphics pipeline.
     */
    void reloadTexture(const std::string& newImagePath);
// ================================================================================
private:
    AllocatorManager& allocatorManager;  /**< The memory allocator manager for handling buffer memory. */
    VkDevice device;                     /**< The Vulkan device handle. */
    VkPhysicalDevice physicalDevice; /**< The Vulkan physical device used for querying memory properties. */ 
    CommandBufferManager& commandBufferManager;  /**< Reference to the command buffer manager. */
    VkQueue graphicsQueue;/**< The Vulkan graphics queue used for submitting commands. */ 
    std::string imagePath; /**< Path to the texture image to be loaded. */ 

    VkImage textureImage = VK_NULL_HANDLE ; /**< The Vulkan image object representing the texture. */ 
    VmaAllocation textureImageMemory = VK_NULL_HANDLE ; /**< The memory backing the Vulkan texture image. */ 
    VkImageView textureImageView = VK_NULL_HANDLE;
    VkSampler textureSampler = VK_NULL_HANDLE;

    std::mutex textureMutex;
// --------------------------------------------------------------------------------

    /**
     * @brief Loads the texture image from a file and uploads it to a Vulkan image.
     *
     * Loads the texture from the specified file, creates a staging buffer for the pixel data, 
     * and uploads it to a Vulkan image. The texture image is then transitioned to the 
     * appropriate layout for shader sampling.
     */
    void createTextureImage();
// --------------------------------------------------------------------------------

    /**
     * @brief Creates a Vulkan image and allocates memory for it.
     *
     * Creates a Vulkan image based on the provided width, height, format, and usage flags.
     * Allocates memory for the image using the specified memory properties.
     *
     * @param width The width of the image.
     * @param height The height of the image.
     * @param format The format of the image (e.g., VK_FORMAT_R8G8B8A8_SRGB).
     * @param tiling The tiling mode of the image (e.g., VK_IMAGE_TILING_OPTIMAL).
     * @param usage The usage flags for the image (e.g., transfer source, sampled image).
     * @param properties The memory properties for the image (e.g., device-local memory).
     * @param image The Vulkan image handle to be created.
     * @param imageMemory The Vulkan memory handle to back the image.
     */
    // void createImage(uint32_t width, uint32_t height, VkFormat format, 
    //                  VkImageTiling tiling, VkImageUsageFlags usage, 
    //                  VkMemoryPropertyFlags properties, VkImage& image, 
    //                  VkDeviceMemory& imageMemory);
    void createImage(uint32_t width, uint32_t height, VkFormat format, 
                     VkImageTiling tiling, VkImageUsageFlags usage, 
                     VmaMemoryUsage memoryUsage, VkImage& image, 
                     VmaAllocation& imageMemory);
// --------------------------------------------------------------------------------

    /**
     * @brief Finds the appropriate memory type for a Vulkan resource.
     *
     * Finds a suitable memory type from the physical device memory properties that 
     * satisfies the required properties for a Vulkan resource.
     *
     * @param typeFilter A bitmask of memory types supported by the resource.
     * @param properties The desired memory properties (e.g., device-local, host-visible).
     * @return The index of the memory type that matches the required properties.
     */
    uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
// --------------------------------------------------------------------------------

    /**
     * @brief Begins recording a single-use command buffer for submitting Vulkan commands.
     *
     * Allocates and begins recording a command buffer that is intended for single-use operations.
     * This is typically used for short-lived operations like image transitions and buffer-to-image copies.
     *
     * @return The Vulkan command buffer handle.
     */
    VkCommandBuffer beginSingleTimeCommands();

// --------------------------------------------------------------------------------

    /**
     * @brief Ends recording and submits the single-use command buffer for execution.
     *
     * Ends the recording of a single-use command buffer and submits it to the graphics queue 
     * for execution. The command buffer is freed after it completes execution.
     *
     * @param commandBuffer The command buffer to end and submit for execution.
     */
    void endSingleTimeCommands(VkCommandBuffer commandBuffer);
// --------------------------------------------------------------------------------

    /**
     * @brief Transitions the image layout to the specified new layout.
     *
     * Transitions a Vulkan image from one layout to another (e.g., from undefined to transfer destination).
     * This function is used during texture uploads and prepares the image for shader access.
     *
     * @param image The Vulkan image to transition.
     * @param format The format of the image.
     * @param oldLayout The current layout of the image.
     * @param newLayout The desired layout of the image after the transition.
     */
    void transitionImageLayout(VkImage image, VkFormat format, 
                               VkImageLayout oldLayout, VkImageLayout newLayout);
// --------------------------------------------------------------------------------

    /**
     * @brief Copies data from a buffer to a Vulkan image.
     *
     * Copies pixel data from a staging buffer to the specified Vulkan image.
     * This function is used during texture uploads to transfer the image data to the GPU.
     *
     * @param buffer The Vulkan buffer containing the pixel data.
     * @param image The Vulkan image to which the data is copied.
     * @param width The width of the image.
     * @param height The height of the image.
     */
    void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
// --------------------------------------------------------------------------------

    /**
     * @brief Checks if the image format has a stencil component.
     *
     * Determines whether a given image format includes a stencil component.
     * This is useful when transitioning image layouts that involve depth-stencil images.
     *
     * @param format The Vulkan format to check.
     * @return True if the format includes a stencil component, false otherwise.
     */
    bool hasStencilComponent(VkFormat format);
// --------------------------------------------------------------------------------

    /**
     * @brief Creates an image view for the texture.
     *
     * This function sets up a Vulkan image view for the texture image, allowing shaders to access
     * the image data. The image view is configured for 2D textures.
     */
    void createTextureImageView();
// --------------------------------------------------------------------------------

    /**
     * @brief Creates an image view for a specific Vulkan image.
     *
     * Sets up a Vulkan image view for the provided image. The view is configured based on the format
     * and is typically used for sampling in shaders.
     *
     * @param image The Vulkan image for which the image view is created.
     * @param format The format of the image.
     * @return The created Vulkan image view.
     */
    VkImageView createImageView(VkImage image, VkFormat format);
// --------------------------------------------------------------------------------

    /**
     * @brief Creates a Vulkan image memory barrier for layout transitions.
     *
     * Sets up a `VkImageMemoryBarrier` structure for transitioning a Vulkan image from one layout to another.
     * This helper function encapsulates common settings, allowing you to specify the source and destination
     * image layouts, format, and image to transition. It simplifies the setup of image layout transitions by
     * returning a pre-configured `VkImageMemoryBarrier` object that can be used in `vkCmdPipelineBarrier`.
     *
     * @param image The Vulkan image to transition.
     * @param format The format of the image (e.g., VK_FORMAT_R8G8B8A8_SRGB).
     * @param oldLayout The current layout of the image.
     * @param newLayout The desired layout of the image after the transition.
     * @return A configured `VkImageMemoryBarrier` for the specified transition.
    */
    VkImageMemoryBarrier createImageMemoryBarrier(VkImage image, VkFormat format, 
                                                  VkImageLayout oldLayout, VkImageLayout newLayout);
// --------------------------------------------------------------------------------

    /**
     * @brief Allocates a single-use Vulkan command buffer.
     *
     * This function allocates a primary command buffer from the command pool managed by the
     * `CommandBufferManager`. The command buffer is intended for short-lived operations and is
     * typically used for one-time commands such as buffer-to-image copies or layout transitions.
     * 
     * The caller is responsible for starting and ending the command buffer recording and freeing
     * the command buffer after use.
     *
     * @return A Vulkan command buffer handle ready for recording.
     */
    VkCommandBuffer allocateCommandBuffer();
// --------------------------------------------------------------------------------

    /**
     * @brief Submits a single-use command buffer and waits for execution to complete.
     *
     * This function is designed to submit a command buffer that performs a single, short-lived operation.
     * After submission, it waits for the command to complete, making it suitable for one-time commands such as
     * buffer-to-image copies or image layout transitions. The function frees the command buffer after execution.
     *
     * @param commandBuffer The command buffer to submit for execution.
     */
    void submitSingleTimeCommandBuffer(VkCommandBuffer commandBuffer);
};
// ================================================================================
// ================================================================================ 

/**
 * @class BufferManager
 * @brief Manages vertex, index, and uniform buffers for Vulkan rendering.
 *
 * This class encapsulates the management and allocation of various Vulkan buffers, such as vertex buffers,
 * index buffers, and uniform buffers. It also handles the mapping and updating of uniform buffers for different
 * frames.
 */
class BufferManager {
public:
     /**
     * @brief Constructor for BufferManager.
     *
     * @param vertices A vector of Vertex objects representing the vertex data.
     * @param indices A vector of 16-bit unsigned integers representing the index data.
     * @param allocatorManager A reference to the AllocatorManager responsible for memory allocation.
     * @param commandBufferManager A reference to the CommandBufferManager used for command buffer management.
     * @param graphicsQueue The Vulkan queue used for submitting graphics commands.
     */
    BufferManager(const std::vector<Vertex>& vertices,
                  const std::vector<uint16_t>& indices,
                  AllocatorManager& allocatorManager,
                  CommandBufferManager& commandBufferManager,
                  VkQueue graphicsQueue);
// --------------------------------------------------------------------------------
    
    /**
     * @brief Destructor for BufferManager.
     *
     * Cleans up all allocated Vulkan buffers and frees any associated memory.
     */
    ~BufferManager();
// --------------------------------------------------------------------------------

    /**
     * @brief Updates the uniform buffer with new data for the current frame.
     *
     * @param currentFrame The index of the current frame.
     * @param ubo The UniformBufferObject containing the new data to be copied to the uniform buffer.
     */
    void updateUniformBuffer(uint32_t currentFrame, const UniformBufferObject& ubo);
// --------------------------------------------------------------------------------

    /**
     * @brief Retrieves the Vulkan vertex buffer.
     *
     * @return The Vulkan buffer used for storing vertex data.
     */
    const VkBuffer getVertexBuffer() const;
// --------------------------------------------------------------------------------

    /**
     * @brief Retrieves the Vulkan index buffer.
     *
     * @return The Vulkan buffer used for storing index data.
     */
    const VkBuffer getIndexBuffer() const;
// --------------------------------------------------------------------------------

    /**
     * @brief Retrieves the vector of uniform buffers used for each frame.
     *
     * @return A reference to the vector of Vulkan uniform buffers.
     */
    const std::vector<VkBuffer>& getUniformBuffers() const;
// --------------------------------------------------------------------------------
    
    /**
     * @brief Retrieves the vector of mapped uniform buffers used for memory access.
     *
     * @return A reference to the vector of void pointers that map the uniform buffers.
     */
    const std::vector<void*>& getUniformBuffersMapped() const;
// ================================================================================
private:
    std::vector<Vertex> vertices;                   /**< The vertex data used for rendering. */
    std::vector<uint16_t> indices;                  /**< The index data for drawing elements. */
    AllocatorManager& allocatorManager;             /**< The memory allocator manager for handling buffer memory. */
    CommandBufferManager& commandBufferManager;     /**< Command buffer manager for managing related command buffers. */
    VkQueue graphicsQueue;                          /**< The Vulkan queue used for submitting graphics commands. */

    VkBuffer vertexBuffer;                          /**< Vulkan buffer for storing vertex data. */
    VkBuffer indexBuffer;                           /**< Vulkan buffer for storing index data. */
    VmaAllocation vertexBufferAllocation;           /**< Memory allocation handle for the vertex buffer. */
    VmaAllocation indexBufferAllocation;            /**< Memory allocation handle for the index buffer. */

    std::vector<VkBuffer> uniformBuffers;           /**< Vector of Vulkan buffers used for uniform data across frames. */
    std::vector<void*> uniformBuffersMapped;        /**< Vector of pointers that map uniform buffers for direct memory access. */
    std::vector<VmaAllocation> uniformBuffersMemory;/**< Memory allocation handles for the uniform buffers. */
// --------------------------------------------------------------------------------

    /**
     * @brief Creates the vertex buffer and allocates memory for it.
     *
     * @return True if the vertex buffer was successfully created, false otherwise.
     */
    bool createVertexBuffer();
// --------------------------------------------------------------------------------

    /**
     * @brief Creates the index buffer and allocates memory for it.
     *
     * @return True if the index buffer was successfully created, false otherwise.
     */
    bool createIndexBuffer();
// --------------------------------------------------------------------------------

    /**
     * @brief Creates uniform buffers for each frame in the application.
     *
     * @return True if the uniform buffers were successfully created, false otherwise.
     */
    bool createUniformBuffers();
};
// // ================================================================================
// // ================================================================================ 

/**
 * @class DescriptorManager
 * @brief Manages Vulkan descriptor sets, layouts, and descriptor pools.
 *
 * This class handles the creation and management of Vulkan descriptor sets, layouts, and descriptor pools.
 * It is responsible for creating the descriptor sets for each frame and managing the descriptor pool and layout.
 */
class DescriptorManager {
public:
    /**
     * @brief Constructor for DescriptorManager.
     *
     * @param device The Vulkan device handle used for creating descriptor sets and pools.
     */
    DescriptorManager(VkDevice device);
// --------------------------------------------------------------------------------

    /**
     * @brief Destructor for DescriptorManager.
     *
     * Cleans up the descriptor pool and descriptor set layout associated with the manager.
     */
    ~DescriptorManager();
// --------------------------------------------------------------------------------

    /**
     * @brief Creates descriptor sets for each frame in the application.
     *
     * This method allocates and configures descriptor sets for each frame, allowing the shaders
     * to access uniform buffer data and texture sampling resources. The descriptor sets
     * are configured to include both the uniform buffer and the texture sampler.
     * 
     * @param uniformBuffers A vector of Vulkan buffers that hold the uniform buffer data for each frame.
     * @param textureImageView The Vulkan image view of the texture to be sampled in the shader.
     * @param textureSampler The Vulkan sampler used to sample the texture image.
     * 
     * @throws std::runtime_error if the descriptor sets cannot be allocated or updated.
     */ 
    void createDescriptorSets(const std::vector<VkBuffer> uniformBuffers,
                              VkImageView textureImageView,
                              VkSampler textureSampler);
// --------------------------------------------------------------------------------

    /**
     * @brief Retrieves the Vulkan descriptor set layout.
     *
     * @return A reference to the Vulkan descriptor set layout.
     */
    const VkDescriptorSetLayout& getDescriptorSetLayout() const;
// --------------------------------------------------------------------------------

    /**
     * @brief Retrieves the Vulkan descriptor pool.
     *
     * @return A reference to the Vulkan descriptor pool.
     */
    const VkDescriptorPool& getDescriptorPool() const;
// --------------------------------------------------------------------------------

    /**
     * @brief Retrieves the vector of Vulkan descriptor sets.
     *
     * @return A reference to the vector of Vulkan descriptor sets.
     */
    const std::vector<VkDescriptorSet>& getDescriptorSets() const;
// --------------------------------------------------------------------------------

    /**
     * @brief Retrieves the descriptor set for a specific frame.
     *
     * @param frameIndex The index of the frame for which to retrieve the descriptor set.
     * @return A reference to the Vulkan descriptor set for the given frame.
     */
    const VkDescriptorSet& getDescriptorSet(uint32_t frameIndex) const;
// ================================================================================
private:
    VkDevice device;                                /**< The Vulkan device handle. */

    VkDescriptorSetLayout descriptorSetLayout = VK_NULL_HANDLE;  /**< The layout of the descriptor sets. */
    VkDescriptorPool descriptorPool = VK_NULL_HANDLE;            /**< The descriptor pool for allocating descriptor sets. */
    std::vector<VkDescriptorSet> descriptorSets;                 /**< A vector holding descriptor sets for each frame. */
// --------------------------------------------------------------------------------

    /**
     * @brief Creates the Vulkan descriptor set layout.
     *
     * Defines the layout of the descriptor sets and creates a corresponding layout object.
     */
    void createDescriptorSetLayout();
// --------------------------------------------------------------------------------

    /**
     * @brief Creates the Vulkan descriptor pool.
     *
     * Allocates a descriptor pool from which descriptor sets can be allocated.
     */
    void createDescriptorPool();
};
// ================================================================================
// ================================================================================ 

/**
 * @class GraphicsPipeline
 * @brief Manages the Vulkan graphics pipeline and associated resources.
 *
 * This class encapsulates the creation and management of the Vulkan graphics pipeline,
 * including framebuffers, render passes, and shader modules. It also handles command buffer
 * recording for rendering.
 */
class GraphicsPipeline {
public:

    /**
     * @brief Constructor for GraphicsPipeline.
     *
     * Initializes the graphics pipeline by setting up the render pass, creating the pipeline layout, 
     * and building the graphics pipeline.
     *
     * @param device The Vulkan logical device handle.
     * @param swapChain Reference to the swap chain.
     * @param commandBufferManager Reference to the CommandBufferManager, used for managing command buffers.
     * @param bufferManager Reference to the BufferManager, which provides vertex and index buffers.
     * @param descriptorManager Reference to the DescriptorManager, which provides descriptor sets and layouts.
     * @param indices The index data for rendering.
     * @param physicalDevice The Vulkan physical device handle.
     * @param vertFile The location of the vertice shader file relative to the executable 
     * @param fragFile The location of the fragmentation shader file relative to the executable
     */
    GraphicsPipeline(VkDevice device,
                     SwapChain& swapChain,
                     CommandBufferManager& commandBufferManager,
                     BufferManager& bufferManager,
                     DescriptorManager& descirptorManager,
                     const std::vector<uint16_t>& indices,
                     VkPhysicalDevice physicalDevice,
                     std::string vertFile,
                     std::string fragFile);
 // --------------------------------------------------------------------------------

    /**
     * @brief Destructor for GraphicsPipeline.
     *
     * Cleans up all Vulkan resources associated with the graphics pipeline,
     * including framebuffers, pipeline layout, and the render pass.
     */
    ~GraphicsPipeline();
// --------------------------------------------------------------------------------

    /**
     * @brief Creates framebuffers for each swap chain image view.
     *
     * @param swapChainImageViews The image views from the swap chain.
     * @param swapChainExtent The extent of the swap chain, i.e., its width and height.
     */
    void createFrameBuffers(const std::vector<VkImageView>& swapChainImageViews, 
                            VkExtent2D swapChainExtent);
// --------------------------------------------------------------------------------

    /**
     * @brief Destroys all the framebuffers.
     *
     * This method cleans up and destroys the Vulkan framebuffers created for the swap chain.
     */
    void destroyFramebuffers();
// --------------------------------------------------------------------------------

    /**
     * @brief Records command buffer for a specific frame and image index.
     *
     * This method records the commands needed to render a frame, including setting up the
     * render pass, binding the graphics pipeline, and drawing indexed vertex data.
     *
     * @param frameIndex The index of the current frame in flight.
     * @param imageIndex The index of the swap chain image being rendered to.
     */
    void recordCommandBuffer(uint32_t frameIndex, uint32_t imageIndex);
// --------------------------------------------------------------------------------

    /**
     * @brief Retrieves the pipeline layout.
     *
     * @return A reference to the Vulkan pipeline layout.
     */
    const VkPipelineLayout& getPipelineLayout() const;
// --------------------------------------------------------------------------------

    /**
     * @brief Retrieves the Vulkan graphics pipeline.
     *
     * @return A reference to the Vulkan graphics pipeline.
     */
    const VkPipeline& getPipeline() const;
// --------------------------------------------------------------------------------

    /**
     * @brief Retrieves the Vulkan render pass.
     *
     * @return A reference to the Vulkan render pass.
     */
    const VkRenderPass& getRenderPass() const;
// --------------------------------------------------------------------------------

    /**
     * @brief Retrieves the vector of Vulkan framebuffers.
     *
     * @return A reference to the vector of Vulkan framebuffers.
     */
    const std::vector<VkFramebuffer>& getFrameBuffers() const;
// --------------------------------------------------------------------------------

    /**
     * @brief Retrieves a specific framebuffer by frame index.
     *
     * @param frameIndex The index of the framebuffer to retrieve.
     * @return A reference to the Vulkan framebuffer.
     */
    const VkFramebuffer& getFrameBuffer(uint32_t frameIndex) const;
// ================================================================================
private:
    VkDevice device;                          /**< Vulkan logical device handle. */
    SwapChain& swapChain;                     /**< Reference to the swap chain. */
    CommandBufferManager& commandBufferManager;/**< Reference to the command buffer manager. */
    BufferManager& bufferManager;             /**< Reference to the buffer manager. */
    DescriptorManager& descriptorManager;     /**< Reference to the descriptor manager. */
    std::vector<uint16_t> indices;            /**< Index data for rendering. */
    VkPhysicalDevice physicalDevice;          /**< Vulkan physical device handle. */
    std::string vertFile;                     /**< Vertices Shader File. */ 
    std::string fragFile;                     /**< Fragmentation Shader File. */

    VkPipelineLayout pipelineLayout;          /**< The Vulkan pipeline layout. */
    VkPipeline graphicsPipeline;              /**< The Vulkan graphics pipeline. */
    VkRenderPass renderPass;                  /**< The Vulkan render pass. */
    std::vector<VkFramebuffer> framebuffers;  /**< Framebuffers for each swap chain image. */
// --------------------------------------------------------------------------------

    /**
     * @brief Creates a shader module from the provided SPIR-V bytecode.
     *
     * @param code The SPIR-V bytecode of the shader.
     * @return The created shader module.
     */
    VkShaderModule createShaderModule(const std::vector<char>& code);
// --------------------------------------------------------------------------------

    /**
     * @brief Reads a file into a vector of characters.
     *
     * @param filename The path to the file.
     * @return The file content as a vector of characters.
     */
    std::vector<char> readFile(const std::string& filename);
// --------------------------------------------------------------------------------

    /**
     * @brief Finds a suitable memory type for Vulkan memory allocations.
     *
     * @param typeFilter A bitmask specifying the memory types to consider.
     * @param properties Required memory properties (e.g., device-local or host-visible).
     * @return The index of a suitable memory type.
     */
    uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
// --------------------------------------------------------------------------------

    /**
     * @brief Creates the Vulkan render pass.
     *
     * @param swapChainImageFormat The format of the swap chain images.
     */
    void createRenderPass(VkFormat swapChainImageFormat);
// --------------------------------------------------------------------------------

    /**
     * @brief Creates the graphics pipeline.
     *
     * Sets up all the pipeline stages, including shaders, input assembly, and rasterization.
     */
    void createGraphicsPipeline();
};
// ================================================================================
// ================================================================================
#endif /* graphics_HPP */
// ================================================================================
// ================================================================================
// eof
