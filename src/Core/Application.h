// File: src/Core/Application.h
#ifndef RUNA_CORE_APPLICATION_H
#define RUNA_CORE_APPLICATION_H


#include "../RunaAPI.h"
#include "Graphics/Window.h"
#include "Graphics/Renderer.h"
#include "Input.h"
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
    Input& getInput() { return *m_input; }

    // Get current FPS (updated every second)
    int getFPS() const { return m_currentFPS; }

private:
    std::unique_ptr<Window> m_window;
    std::unique_ptr<Renderer> m_renderer;
    std::unique_ptr<Input> m_input;
    bool m_running = false;
    int m_currentFPS = 0;

    void mainLoop();
};

} // namespace Runa
#endif // RUNA_CORE_APPLICATION_H