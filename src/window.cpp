#include "window.hpp"
#include <stdexcept>

Window::Window(int width, int height, std::string_view title) {
    // Remove if creating 2 windows later.
    if (!glfwInit()) {
        throw std::runtime_error("Failed to initialize GLFW");
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    GLFWwindow* glWindow = glfwCreateWindow(width, height, title.data(), nullptr, nullptr);

    if(!glWindow) {
        glfwTerminate();
        throw std::runtime_error("Failed to create window");
    }

    handle = glWindow;
}

Window::Window(Window&& other) noexcept
   : handle(other.handle) 
{
    other.handle = nullptr;
}

Window& Window::operator=(Window&& other) noexcept {
    if (this != &other) {
        if(handle) {
            glfwDestroyWindow(handle);
        }
        handle = other.handle;
        other.handle = nullptr;
    }
    return *this;
}

Window::~Window() {
    if (handle) {
        glfwDestroyWindow(handle);
        // This also closes all windows if we have multiple windows
        glfwTerminate();
    }
}

bool Window::shouldClose() const {
    return glfwWindowShouldClose(handle);
}

void Window::pollEvents(){
    glfwPollEvents();
}


