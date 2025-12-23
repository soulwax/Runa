// File: src/Core/Application.h

#pragma once

#include "../RunaAPI.h"
#include "../Graphics/Window.h"
#include "../Graphics/Renderer.h"
#include <memory>

namespace Runa {

class RUNA_API Application {
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
    
    // Get current FPS (updated every second)
    int getFPS() const { return m_currentFPS; }

private:
    std::unique_ptr<Window> m_window;
    std::unique_ptr<Renderer> m_renderer;
    bool m_running = false;
    int m_currentFPS = 0;

    void mainLoop();
};

} // namespace Runa
