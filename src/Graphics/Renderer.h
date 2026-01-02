// File: src/Graphics/Renderer.h

#ifndef RUNA_GRAPHICS_RENDERER_H
#define RUNA_GRAPHICS_RENDERER_H

#include "RunaAPI.h"
#include "Window.h"
#include <VK2D/VK2D.h>
#include <SDL3/SDL.h>

namespace Runa {

class RUNA_API Renderer {
public:
  explicit Renderer(Window &window);
  ~Renderer();

  // Disable copying
  Renderer(const Renderer &) = delete;
  Renderer &operator=(const Renderer &) = delete;

  // Disable moving (VK2DRenderer is not trivially movable)
  Renderer(Renderer &&) noexcept = delete;
  Renderer &operator=(Renderer &&) noexcept = delete;

  // Vulkan2D renderer access
  VK2DRenderer getVK2DRenderer() const { return m_vk2dRenderer; }
  bool isValid() const { return m_vk2dRenderer != nullptr; }
  Window &getWindow() { return m_window; }

  void beginFrame();
  void endFrame();
  void clear(float r, float g, float b, float a);

  // Get clear color (for API compatibility)
  struct ClearColor {
    float r = 0.0f, g = 0.0f, b = 0.0f, a = 1.0f;
  };
  ClearColor getClearColor() const { return m_clearColor; }

private:
  Window &m_window;
  VK2DRenderer m_vk2dRenderer = nullptr;
  ClearColor m_clearColor{0.05f, 0.05f, 0.1f, 1.0f};
};

} // namespace Runa

#endif // RUNA_GRAPHICS_RENDERER_H
