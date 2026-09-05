#pragma once

#include <memory>

class Window;
class Renderer;

class Application {
    std::unique_ptr<Window> window;
    std::unique_ptr<Renderer> renderer;
public:
    Application();
    ~Application();
    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;
    void run();
};
