#pragma once

#ifdef USE_VOLK
#include <volk.h>
#else
#include <vulkan/vulkan.h>
#endif

struct GLFWwindow;

class Renderer {
    VkInstance instance = VK_NULL_HANDLE;
    void createInstance();

public:
    explicit Renderer(GLFWwindow* window);
    ~Renderer();

    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;

    void drawFrame();
};
