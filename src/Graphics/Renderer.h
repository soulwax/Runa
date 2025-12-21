// File: src/Graphics/Renderer.h

#pragma once

#include "../RunaAPI.h"
#include "Window.h"
#include "Shader.h"
#include <SDL3/SDL.h>
#include <memory>

namespace Runa {

class RUNA_API Renderer {
public:
    explicit Renderer(Window& window);
    ~Renderer();

    // Disable copying
    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;

    // Enable moving
    Renderer(Renderer&&) noexcept = default;
    Renderer& operator=(Renderer&&) noexcept = default;

    SDL_GPUDevice* getDevice() const { return m_device; }
    bool isValid() const { return m_device != nullptr; }
    Window& getWindow() { return m_window; }

    void beginFrame();
    void endFrame();
    void clear(float r, float g, float b, float a);

    // Shader management
    std::shared_ptr<Shader> createShader(const std::string& vertexPath, const std::string& fragmentPath);

private:
    Window& m_window;
    SDL_GPUDevice* m_device = nullptr;
    SDL_GPUTexture* m_swapchainTexture = nullptr;

    void acquireSwapchainTexture();
};

} // namespace Runa
