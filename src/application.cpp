#include "application.hpp"
#include "window.hpp"
#include "renderer.hpp"

Application::Application()
    : window(std::make_unique<Window>(1920, 1080, "Editor"))
      , renderer(std::make_unique<Renderer>(window->getHandle()))
{
}

void Application::run() {
    while (!window->shouldClose()) {
        window->pollEvents();
    }
}

Application::~Application() {
}
