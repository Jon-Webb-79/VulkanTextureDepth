# cmake_minimum_required(VERSION 3.29.6)
# project(VulkanApplication)
#
# set(CMAKE_CXX_STANDARD 17)
# set(CMAKE_CXX_STANDARD_REQUIRED ON)
#
# # Set vcpkg toolchain
# set(CMAKE_TOOLCHAIN_FILE "/home/jonwebb/Code_Dev/C++/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain file")
#
# # Option to build unit tests
# option(BUILD_TESTS "Build the unit tests" OFF)
#
# # ExternalProject module for external dependencies (GLFW)
# include(ExternalProject)
#
# # Define GLFW external project
# ExternalProject_Add(
#     glfw
#     GIT_REPOSITORY https://github.com/glfw/glfw.git
#     GIT_TAG latest
#     CMAKE_ARGS -DGLFW_BUILD_EXAMPLES=OFF
#                -DGLFW_BUILD_TESTS=OFF
#                -DGLFW_BUILD_DOCS=OFF
#                -DGLFW_BUILD_INSTALL=OFF
#                -DGLFW_BUILD_WAYLAND=OFF
#                -DGLFW_BUILD_X11=ON
#     PREFIX ${CMAKE_BINARY_DIR}/glfw
#     INSTALL_COMMAND ""
# )
#
# # Find Vulkan and VulkanMemoryAllocator
# find_package(Vulkan REQUIRED)
# find_package(VulkanMemoryAllocator CONFIG REQUIRED)
#
# # Find glslangValidator
# find_program(GLSLANG_VALIDATOR glslangValidator)
# if (NOT GLSLANG_VALIDATOR)
#     message(FATAL_ERROR "glslangValidator not found! Install it to compile shaders.")
# endif()
#
# # Shader compilation
# set(SHADERS
#     ${CMAKE_SOURCE_DIR}/shaders/shader.vert
#     ${CMAKE_SOURCE_DIR}/shaders/shader.frag
# )
# foreach(SHADER ${SHADERS})
#     get_filename_component(FILE_NAME ${SHADER} NAME)
#     set(SPIRV ${CMAKE_SOURCE_DIR}/shaders/${FILE_NAME}.spv)
#
#     add_custom_command(
#         OUTPUT ${SPIRV}
#         COMMAND ${GLSLANG_VALIDATOR} -V ${SHADER} -o ${SPIRV}
#         DEPENDS ${SHADER}
#         COMMENT "Compiling ${SHADER} to SPIR-V"
#     )
#
#     list(APPEND SPIRV_SHADERS ${SPIRV})
# endforeach()
#
# # Add custom target to build shaders
# add_custom_target(ShadersTarget ALL DEPENDS ${SPIRV_SHADERS})
#
# # Create a library for shared code
# add_library(VulkanApplicationLib
#     application.cpp
#     validation_layers.cpp
#     queues.cpp
#     devices.cpp
#     graphics.cpp
#     memory.cpp
# )
#
# # Define the main executable using the shared library
# add_executable(VulkanApplication main.cpp)
#
# add_dependencies(VulkanApplication ShadersTarget)
#
# # Include GLFW, Vulkan, and VMA directories
# ExternalProject_Get_Property(glfw source_dir binary_dir)
# target_include_directories(VulkanApplication PRIVATE 
#     ${source_dir}/include 
#     ${Vulkan_INCLUDE_DIRS} 
#     ${VulkanMemoryAllocator_INCLUDE_DIRS}
# )
#
# # Link GLFW, Vulkan, VMA, and the shared library
# add_dependencies(VulkanApplication glfw)
# target_link_libraries(VulkanApplication PRIVATE
#     VulkanApplicationLib
#     ${binary_dir}/src/libglfw3.a 
#     Vulkan::Vulkan 
#     GPUOpen::VulkanMemoryAllocator 
#     dl pthread X11 Xxf86vm Xrandr Xi
# )
#
# # Set the output directory for the executable
# set_target_properties(VulkanApplication PROPERTIES
#     RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
# )
#
# # Debug and release flags
# set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -Wpedantic -g")
# set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall -Werror -Wpedantic -O2")
#
# # Fetch GoogleTest if unit tests are enabled
# if(BUILD_TESTS)
#     enable_testing()
#
#     # Fetch GoogleTest
#     include(FetchContent)
#     FetchContent_Declare(
#         googletest
#         URL https://github.com/google/googletest/archive/release-1.12.1.tar.gz
#     )
#     FetchContent_MakeAvailable(googletest)
#
#     # Add the test directory
#     add_subdirectory(test)
# endif()


cmake_minimum_required(VERSION 3.29.6)
project(VulkanApplication)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set vcpkg toolchain
set(CMAKE_TOOLCHAIN_FILE "/home/jonwebb/Code_Dev/C++/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain file")

# Include ExternalProject module to manage external dependencies
include(ExternalProject)

# Define GLFW as an external project
ExternalProject_Add(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG latest
    CMAKE_ARGS -DGLFW_BUILD_EXAMPLES=OFF
               -DGLFW_BUILD_TESTS=OFF
               -DGLFW_BUILD_DOCS=OFF
               -DGLFW_BUILD_INSTALL=OFF
               -DGLFW_BUILD_WAYLAND=OFF
               -DGLFW_BUILD_X11=ON
    PREFIX ${CMAKE_BINARY_DIR}/glfw
    INSTALL_COMMAND ""
)

# Find the Vulkan package
find_package(Vulkan REQUIRED)
find_package(VulkanMemoryAllocator CONFIG REQUIRED)

# Find glslangValidator
find_program(GLSLANG_VALIDATOR glslangValidator)
if (NOT GLSLANG_VALIDATOR)
    message(FATAL_ERROR "glslangValidator not found! Install it to compile shaders.")
endif()

# Shader files
set(SHADERS
    ${CMAKE_SOURCE_DIR}/shaders/shader.vert
    ${CMAKE_SOURCE_DIR}/shaders/shader.frag
)

# Compile shaders to SPIR-V (set the SPIRV output to be in the source directory)
foreach(SHADER ${SHADERS})
    get_filename_component(FILE_NAME ${SHADER} NAME)
    set(SPIRV ${CMAKE_SOURCE_DIR}/shaders/${FILE_NAME}.spv)  # Output to the source directory

    add_custom_command(
        OUTPUT ${SPIRV}
        COMMAND ${GLSLANG_VALIDATOR} -V ${SHADER} -o ${SPIRV}
        DEPENDS ${SHADER}
        COMMENT "Compiling ${SHADER} to SPIR-V"
    )

    list(APPEND SPIRV_SHADERS ${SPIRV})
endforeach()

# Add custom target to build all shaders
add_custom_target(ShadersTarget ALL DEPENDS ${SPIRV_SHADERS})

# Define the executable
add_executable(VulkanApplication 
               main.cpp
               application.cpp
               validation_layers.cpp
               queues.cpp
               devices.cpp
               graphics.cpp
               # graphics_pipeline.cpp
               memory.cpp
)

# Make VulkanApplication dependent on ShadersTarget
add_dependencies(VulkanApplication ShadersTarget)

# Include GLFW and Vulkan directories
ExternalProject_Get_Property(glfw source_dir binary_dir)
target_include_directories(VulkanApplication PRIVATE ${source_dir}/include ${Vulkan_INCLUDE_DIRS})

# Link the GLFW, Vulkan, and VMA libraries and add the necessary linker flags
add_dependencies(VulkanApplication glfw)
target_link_libraries(VulkanApplication PRIVATE 
    ${binary_dir}/src/libglfw3.a 
    Vulkan::Vulkan 
    GPUOpen::VulkanMemoryAllocator 
    dl pthread X11 Xxf86vm Xrandr Xi
)

# Set the output directory for the executable
set_target_properties(VulkanApplication PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)

# Additional flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native -mtune=native -fPIE")

# Set debug-specific compiler flags
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -Wpedantic -g")
# Set release-specific compiler flags
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall -Werror -Wpedantic -O2")
