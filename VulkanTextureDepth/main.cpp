// ================================================================================
// ================================================================================
// - File:    main.cpp
// - Purpose: Describe the file purpose here
//
// Source Metadata
// - Author:  Jonathan A. Webb
// - Date:    July 26, 2024
// - Version: 1.0
// - Copyright: Copyright 2022, Jon Webb Inc.
// ================================================================================
// ================================================================================
// Include modules here

#include "include/application.hpp"
#include <iostream>
#include <stdexcept>
#include <memory>
// ================================================================================
// ================================================================================ 

GLFWwindow* create_window(uint32_t h, uint32_t w, const std::string& screen_title, bool full_screen) {
    if (!glfwInit()) {
        throw std::runtime_error("GLFW Initialization Failed!\n");
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    GLFWmonitor* monitor = full_screen ? glfwGetPrimaryMonitor() : nullptr;

    GLFWwindow* window = glfwCreateWindow(w, h, screen_title.c_str(), monitor, nullptr);

    if (!window) {
        glfwTerminate();
        throw std::runtime_error("GLFW Instantiation failed!\n");
    }
    return window;
}
// ================================================================================
// ================================================================================ 


// Begin code
int main(int argc, const char * argv[]) {
    // Define vertices for two connected triangles  
    const std::vector<Vertex> vertices = {
        {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
        {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
        {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
        {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}}
    };
    // const std::vector<Vertex> vertices = {
    //     {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
    //     {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
    //     {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
    //     {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}
    // }; 
    const std::vector<uint16_t> indices = {
        0, 1, 2, 2, 3, 0
    }; 
    
    // Call Application 
    try {
        GLFWwindow* window = create_window(750, 900, "Vulkan Application", false);
        VulkanApplication triangle(window, vertices, indices);

        triangle.run();

         // Clean up the GLFW window
        glfwDestroyWindow(window);

        // Terminate GLFW and clean up resources
        glfwTerminate();
    } catch(const std::exception& e) {
        std::cerr << e.what() <<  "\n";
        return EXIT_FAILURE;
    }
	return EXIT_SUCCESS;
}
// ================================================================================
// ================================================================================
// eof

