// File: src/Application.h

#pragma once

#include "Window.h"
#include "Renderer.h"
#include <memory>

namespace Runa {

class Application {
public:
    Application(const std::string& title, int width, int height);
    virtual ~Application();

    void run();

protected:
    virtual void onInit() {}
    virtual void onUpdate(float deltaTime) {}
    virtual void onRender() {}
    virtual void onShutdown() {}

    Window& getWindow() { return *m_window; }
    Renderer& getRenderer() { return *m_renderer; }

private:
    std::unique_ptr<Window> m_window;
    std::unique_ptr<Renderer> m_renderer;
    bool m_running = false;

    void mainLoop();
};

} // namespace Runa
