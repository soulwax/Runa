// File: src/Graphics/Renderer.cpp

#include "Renderer.h"
#include "Core/Log.h"
#include <stdexcept>

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
    // If we have a swapchain but no sprites were rendered, we still need to submit
    // a command buffer to present the cleared frame
    if (m_swapchainTexture && m_needsClear) {
        // SpriteBatch didn't submit, so we need to submit the clear ourselves
        SDL_GPUCommandBuffer* cmdBuffer = SDL_AcquireGPUCommandBuffer(m_device);
        if (cmdBuffer) {
            SDL_GPUColorTargetInfo colorTarget{};
            colorTarget.texture = m_swapchainTexture;
            colorTarget.clear_color.r = m_clearColor.r;
            colorTarget.clear_color.g = m_clearColor.g;
            colorTarget.clear_color.b = m_clearColor.b;
            colorTarget.clear_color.a = m_clearColor.a;
            colorTarget.load_op = SDL_GPU_LOADOP_CLEAR;
            colorTarget.store_op = SDL_GPU_STOREOP_STORE;
            
            SDL_GPURenderPass* renderPass = SDL_BeginGPURenderPass(cmdBuffer, &colorTarget, 1, nullptr);
            if (renderPass) {
                SDL_EndGPURenderPass(renderPass);
            }
            SDL_SubmitGPUCommandBuffer(cmdBuffer);
        }
    }
    
    // Reset swapchain texture - it will be acquired again next frame
    m_swapchainTexture = nullptr;
    m_needsClear = false;  // Reset clear flag for next frame
}

void Renderer::clear(float r, float g, float b, float a) {
    if (!m_swapchainTexture) {
        LOG_WARN("Cannot clear: no swapchain texture acquired");
        return;
    }

    // Store clear color for later use in render pass
    // The actual clearing will happen in the render pass with LOADOP_CLEAR
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
        SDL_GPUCommandBuffer* cmdBuffer = SDL_AcquireGPUCommandBuffer(m_device);
        if (!cmdBuffer) {
            LOG_ERROR("Failed to acquire command buffer: {}", SDL_GetError());
            return;
        }

        if (!SDL_AcquireGPUSwapchainTexture(cmdBuffer, m_window.getHandle(), &m_swapchainTexture, nullptr, nullptr)) {
            LOG_ERROR("Failed to acquire swapchain texture: {}", SDL_GetError());
            SDL_CancelGPUCommandBuffer(cmdBuffer);
            return;
        }

        if (!m_swapchainTexture) {
            LOG_ERROR("Swapchain texture is null after acquire");
        } else {
            // Submit the command buffer that acquired the texture
            SDL_SubmitGPUCommandBuffer(cmdBuffer);
        }
    }
}

} // namespace Runa
