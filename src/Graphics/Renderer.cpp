// File: src/Graphics/Renderer.cpp

#include "runapch.h"
#include "Renderer.h"
#include "Core/Log.h"

namespace Runa {

Renderer::Renderer(Window& window) : m_window(window) {
    // Create GPU device
    m_device = SDL_CreateGPUDevice(
        SDL_GPU_SHADERFORMAT_SPIRV,
        true,  // debugMode
        nullptr
    );

    if (!m_device) {
        throw std::runtime_error(std::string("Failed to create GPU device: ") + SDL_GetError());
    }

    // Claim the window for GPU operations
    if (!SDL_ClaimWindowForGPUDevice(m_device, m_window.getHandle())) {
        SDL_DestroyGPUDevice(m_device);
        throw std::runtime_error(std::string("Failed to claim window for GPU: ") + SDL_GetError());
    }
    
    // Configure swapchain to support sampling (needed for post-process effects)
    SDL_GPUSwapchainComposition swapchainComposition = SDL_GPU_SWAPCHAINCOMPOSITION_SDR;
    SDL_GPUPresentMode presentMode = SDL_GPU_PRESENTMODE_VSYNC;
    
    if (!SDL_SetGPUSwapchainParameters(m_device, m_window.getHandle(), swapchainComposition, presentMode)) {
        LOG_WARN("Failed to set swapchain parameters: {}", SDL_GetError());
    }

    LOG_INFO("Renderer initialized with SDL3 GPU backend");
    LOG_INFO("GPU Driver: {}", SDL_GetGPUDeviceDriver(m_device));
}

Renderer::~Renderer() {
    if (m_device) {
        SDL_ReleaseWindowFromGPUDevice(m_device, m_window.getHandle());
        SDL_DestroyGPUDevice(m_device);
        LOG_INFO("Renderer destroyed");
    }
}

void Renderer::beginFrame() {
    acquireSwapchainTexture();
}

void Renderer::endFrame() {
    // Clear flag is managed by SpriteBatch::end() which calls clearApplied()
    // If SpriteBatch wasn't used, we might need to clear here, but for now
    // we assume SpriteBatch handles all clearing
    // The flag will be reset by clearApplied() or set again next frame
}

void Renderer::clear(float r, float g, float b, float a) {
    // Store clear color for later use in render pass
    // The actual clearing will happen in the render pass with LOADOP_CLEAR
    // Note: We don't check for swapchain here because it will be acquired in SpriteBatch::end()
    m_clearColor.r = r;
    m_clearColor.g = g;
    m_clearColor.b = b;
    m_clearColor.a = a;
    m_needsClear = true;
}

std::shared_ptr<Shader> Renderer::createShader(const std::string& vertexPath, const std::string& fragmentPath) {
    try {
        return std::make_shared<Shader>(m_device, vertexPath, fragmentPath);
    } catch (const std::exception& e) {
        LOG_ERROR("Failed to create shader: {}", e.what());
        return nullptr;
    }
}

SDL_GPUCommandBuffer* Renderer::acquireCommandBuffer() {
    return SDL_AcquireGPUCommandBuffer(m_device);
}

void Renderer::acquireSwapchainTexture() {
    if (!m_swapchainTexture) {
        // Don't acquire here - let SpriteBatch acquire it on its command buffer
        // This ensures the swapchain is acquired on the same command buffer that uses it
        // For now, we'll acquire it lazily when needed
    }
}

} // namespace Runa
