#pragma once

#include <GLFW/glfw3.h>
#include <string_view>

class Window {
    GLFWwindow* handle = nullptr;
public:
    Window(int width, int height, std::string_view title);
    ~Window();

    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;
    Window(Window&&) noexcept;
    Window& operator=(Window&&) noexcept;

    [[nodiscard]] bool shouldClose() const;
    void pollEvents();
    [[nodiscard]] GLFWwindow* getHandle() const { return handle; }
};

