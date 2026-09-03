#ifdef USE_VOLK
#define VOLK_IMPLEMENTATION
#include <volk.h>
#define GLFW_INCLUDE_NONE
#else
#define GLFW_INCLUDE_VULKAN
#endif

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <cstdlib>
#include <iostream>

int main(int argc, char** argv) {
#ifdef USE_VOLK
    if (volkInitialize() != VK_SUCCESS) {
        return EXIT_FAILURE;
    }
#endif

    if (!glfwInit()) {
        return EXIT_FAILURE;
    }

    std::cout << "Hello World!\n";

    glfwTerminate();
    return EXIT_SUCCESS;
}
