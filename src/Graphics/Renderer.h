// File: src/Graphics/Renderer.h

#ifndef RUNA_GRAPHICS_RENDERER_H
#define RUNA_GRAPHICS_RENDERER_H

#include "RunaAPI.h"
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
    SDL_GPUCommandBuffer* acquireCommandBuffer();

    void beginFrame();
    void endFrame();
    void clear(float r, float g, float b, float a);

    // Shader management
    std::shared_ptr<Shader> createShader(const std::string& vertexPath, const std::string& fragmentPath);

    // Get clear color (used by SpriteBatch)
    struct ClearColor {
        float r = 0.0f, g = 0.0f, b = 0.0f, a = 1.0f;
    };
    ClearColor getClearColor() const { return m_clearColor; }
    bool needsClear() const { return m_needsClear; }
    void clearApplied() { m_needsClear = false; }

private:
    Window& m_window;
    SDL_GPUDevice* m_device = nullptr;
    SDL_GPUTexture* m_swapchainTexture = nullptr;
    ClearColor m_clearColor{0.05f, 0.05f, 0.1f, 1.0f};
    bool m_needsClear = false;

    void acquireSwapchainTexture();
};

} // namespace Runa

#endif // RUNA_GRAPHICS_RENDERER_H
