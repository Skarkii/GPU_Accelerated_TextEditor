#ifdef USE_VOLK
#define VOLK_IMPLEMENTATION
#include <volk.h>
#define GLFW_INCLUDE_NONE
#else
#define GLFW_INCLUDE_VULKAN
#endif

#include <GLFW/glfw3.h>

#include "renderer.hpp"

#include <stdexcept>
#include <string>
#include <vector>

Renderer::Renderer(GLFWwindow* window) {
#ifdef USE_VOLK
    if (volkInitialize() != VK_SUCCESS) {
        throw std::runtime_error("Failed to initialize volk");
    }
#endif

    createInstance();
}

Renderer::~Renderer() {
    if (instance != VK_NULL_HANDLE) {
        vkDestroyInstance(instance, nullptr);
    }
}

void Renderer::createInstance() {
    VkApplicationInfo appInfo{};

    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Text Editor";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    if (glfwExtensions == nullptr) {
        throw std::runtime_error("GLFW: Vulkan not supported");
    }

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;
    createInfo.enabledExtensionCount = glfwExtensionCount;
    createInfo.ppEnabledExtensionNames = glfwExtensions;  // THIS WAS MISSING
    createInfo.enabledLayerCount = 0;

    VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);
    if (result != VK_SUCCESS) {
        throw std::runtime_error("Failed to create Vulkan instance. Error: " + std::to_string(result));
    }

#ifdef USE_VOLK
    volkLoadInstance(instance);
#endif
}

void Renderer::drawFrame() {
}

