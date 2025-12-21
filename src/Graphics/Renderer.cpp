// File: src/Renderer.cpp

#include "Renderer.h"
#include <iostream>
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

    std::cout << "Renderer initialized with SDL3 GPU backend" << std::endl;
    std::cout << "GPU Driver: " << SDL_GetGPUDeviceDriver(m_device) << std::endl;
}

Renderer::~Renderer() {
    if (m_device) {
        SDL_ReleaseWindowFromGPUDevice(m_device, m_window.getHandle());
        SDL_DestroyGPUDevice(m_device);
        std::cout << "Renderer destroyed" << std::endl;
    }
}

void Renderer::beginFrame() {
    acquireSwapchainTexture();
}

void Renderer::endFrame() {
    if (m_swapchainTexture) {
        // Create command buffer
        SDL_GPUCommandBuffer* cmdBuffer = SDL_AcquireGPUCommandBuffer(m_device);
        if (!cmdBuffer) {
            std::cerr << "Failed to acquire command buffer: " << SDL_GetError() << std::endl;
            return;
        }

        // Submit and present
        SDL_SubmitGPUCommandBuffer(cmdBuffer);
        m_swapchainTexture = nullptr;
    }
}

void Renderer::clear(float r, float g, float b, float a) {
    if (!m_swapchainTexture) {
        std::cerr << "Cannot clear: no swapchain texture acquired" << std::endl;
        return;
    }

    SDL_GPUCommandBuffer* cmdBuffer = SDL_AcquireGPUCommandBuffer(m_device);
    if (!cmdBuffer) {
        std::cerr << "Failed to acquire command buffer for clear: " << SDL_GetError() << std::endl;
        return;
    }

    // Create color target info
    SDL_GPUColorTargetInfo colorTargetInfo{};
    colorTargetInfo.texture = m_swapchainTexture;
    colorTargetInfo.clear_color.r = r;
    colorTargetInfo.clear_color.g = g;
    colorTargetInfo.clear_color.b = b;
    colorTargetInfo.clear_color.a = a;
    colorTargetInfo.load_op = SDL_GPU_LOADOP_CLEAR;
    colorTargetInfo.store_op = SDL_GPU_STOREOP_STORE;

    // Begin render pass
    SDL_GPURenderPass* renderPass = SDL_BeginGPURenderPass(
        cmdBuffer,
        &colorTargetInfo,
        1,
        nullptr
    );

    if (renderPass) {
        SDL_EndGPURenderPass(renderPass);
    } else {
        std::cerr << "Failed to begin render pass: " << SDL_GetError() << std::endl;
    }

    SDL_SubmitGPUCommandBuffer(cmdBuffer);
}

std::shared_ptr<Shader> Renderer::createShader(const std::string& vertexPath, const std::string& fragmentPath) {
    try {
        return std::make_shared<Shader>(m_device, vertexPath, fragmentPath);
    } catch (const std::exception& e) {
        std::cerr << "Failed to create shader: " << e.what() << std::endl;
        return nullptr;
    }
}

void Renderer::acquireSwapchainTexture() {
    if (!m_swapchainTexture) {
        SDL_GPUCommandBuffer* cmdBuffer = SDL_AcquireGPUCommandBuffer(m_device);
        if (!cmdBuffer) {
            std::cerr << "Failed to acquire command buffer: " << SDL_GetError() << std::endl;
            return;
        }

        if (!SDL_AcquireGPUSwapchainTexture(cmdBuffer, m_window.getHandle(), &m_swapchainTexture, nullptr, nullptr)) {
            std::cerr << "Failed to acquire swapchain texture: " << SDL_GetError() << std::endl;
            SDL_CancelGPUCommandBuffer(cmdBuffer);
            return;
        }

        if (!m_swapchainTexture) {
            std::cerr << "Swapchain texture is null after acquire" << std::endl;
        }
    }
}

} // namespace Runa
