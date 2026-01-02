// File: src/Graphics/Renderer.cpp

#include "Renderer.h"
#include "../runapch.h"
#include "Core/Log.h"

namespace Runa {

Renderer::Renderer(Window &window) : m_window(window) {
  // Configure Vulkan2D renderer
  VK2DRendererConfig config = {};
  config.msaa = VK2D_MSAA_1X;  // No MSAA for pixel-perfect rendering
  config.screenMode = VK2D_SCREEN_MODE_VSYNC;  // VSync like original
  config.filterMode = VK2D_FILTER_TYPE_NEAREST;  // Pixel-perfect filtering

  // Initialize Vulkan2D renderer
  VK2DResult result = vk2dRendererInit(m_window.getHandle(), config, nullptr);

  if (result != VK2D_SUCCESS) {
    throw std::runtime_error("Failed to create Vulkan2D renderer");
  }

  m_vk2dRenderer = vk2dRendererGetPointer();

  if (!m_vk2dRenderer) {
    throw std::runtime_error("Failed to get Vulkan2D renderer pointer");
  }

  LOG_INFO("Renderer initialized with Vulkan2D backend");
  LOG_INFO("Vulkan2D Config - MSAA: OFF, ScreenMode: VSYNC, Filter: NEAREST");
}

Renderer::~Renderer() {
  if (m_vk2dRenderer) {
    vk2dRendererWait();  // Ensure GPU is idle before cleanup
    vk2dRendererQuit();
    m_vk2dRenderer = nullptr;
    LOG_INFO("Renderer destroyed");
  }
}

void Renderer::beginFrame() {
  // Convert clear color to Vulkan2D format (vec4 = float[4])
  vec4 clearColorArray = {m_clearColor.r, m_clearColor.g, m_clearColor.b, m_clearColor.a};
  vk2dRendererStartFrame(clearColorArray);
}

void Renderer::endFrame() {
  vk2dRendererEndFrame();
}

void Renderer::clear(float r, float g, float b, float a) {
  // Store clear color for next frame (Vulkan2D clears at frame start)
  m_clearColor.r = r;
  m_clearColor.g = g;
  m_clearColor.b = b;
  m_clearColor.a = a;
}

} // namespace Runa
