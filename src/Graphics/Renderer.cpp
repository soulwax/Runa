

#include "Renderer.h"
#include "../runapch.h"
#include "Core/Log.h"

namespace Runa {

Renderer::Renderer(Window &window) : m_window(window) {

  VK2DRendererConfig config = {};
  config.msaa = VK2D_MSAA_1X;
  config.screenMode = VK2D_SCREEN_MODE_VSYNC;  // Enable VSYNC
  config.filterMode = VK2D_FILTER_TYPE_NEAREST;


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
    vk2dRendererWait();
    vk2dRendererQuit();
    m_vk2dRenderer = nullptr;
    LOG_INFO("Renderer destroyed");
  }
}

void Renderer::beginFrame() {

  vec4 clearColorArray = {m_clearColor.r, m_clearColor.g, m_clearColor.b, m_clearColor.a};
  vk2dRendererStartFrame(clearColorArray);
}

void Renderer::endFrame() {
  vk2dRendererEndFrame();
}

void Renderer::clear(float r, float g, float b, float a) {

  m_clearColor.r = r;
  m_clearColor.g = g;
  m_clearColor.b = b;
  m_clearColor.a = a;
}

}
