// File: src/Graphics/PostProcess.cpp

#include "PostProcess.h"
#include "Core/Log.h"
#include <SDL3/SDL.h>

namespace Runa {

PostProcess::PostProcess(Renderer& renderer)
    : m_renderer(renderer) {
    initializeShader();
}

void PostProcess::initializeShader() {
    try {
        m_psychedelicShader = m_renderer.createShader("Resources/shaders/compiled/psychedelic.vert.spv", "Resources/shaders/compiled/psychedelic.frag.spv");
        if (!m_psychedelicShader || !m_psychedelicShader->isValid()) {
            LOG_ERROR("Failed to load psychedelic shader");
        } else {
            LOG_INFO("Psychedelic shader loaded successfully");
        }
    } catch (const std::exception& e) {
        LOG_ERROR("Error initializing psychedelic shader: {}", e.what());
    }
}

void PostProcess::applyPsychedelic(SDL_GPUCommandBuffer* cmdBuffer, SDL_GPUTexture* sourceTexture, SDL_GPUTexture* targetTexture, float time) {
    if (!m_psychedelicShader || !m_psychedelicShader->isValid() || !sourceTexture || !targetTexture) {
        // Fallback: just blit the source to target without effect
        SDL_GPUBlitInfo blitInfo{};
        SDL_zero(blitInfo);
        blitInfo.source.texture = sourceTexture;
        blitInfo.source.w = 1280;  // TODO: Get from texture
        blitInfo.source.h = 720;
        blitInfo.destination.texture = targetTexture;
        blitInfo.destination.w = 1280;
        blitInfo.destination.h = 720;
        blitInfo.load_op = SDL_GPU_LOADOP_DONT_CARE;
        blitInfo.filter = SDL_GPU_FILTER_LINEAR;
        SDL_BlitGPUTexture(cmdBuffer, &blitInfo);
        return;
    }

    renderFullScreenQuad(cmdBuffer, sourceTexture, targetTexture, time);
}

void PostProcess::renderFullScreenQuad(SDL_GPUCommandBuffer* cmdBuffer, SDL_GPUTexture* sourceTexture, SDL_GPUTexture* targetTexture, float time) {
    // Apply psychedelic effect using multiple blits with warping
    // This creates a simple distortion effect
    
    int width = 1280;
    int height = 720;
    
    // Create multiple blits with slight offsets to create warping effect
    float wave1 = sin(time * 2.0f) * 5.0f;
    float wave2 = cos(time * 1.5f) * 3.0f;
    
    // Main blit with slight distortion
    SDL_GPUBlitInfo blitInfo{};
    SDL_zero(blitInfo);
    
    // Source with slight offset for warping
    blitInfo.source.texture = sourceTexture;
    blitInfo.source.x = static_cast<Uint32>(wave1);
    blitInfo.source.y = static_cast<Uint32>(wave2);
    blitInfo.source.w = width;
    blitInfo.source.h = height;
    
    blitInfo.destination.texture = targetTexture;
    blitInfo.destination.x = 0;
    blitInfo.destination.y = 0;
    blitInfo.destination.w = width;
    blitInfo.destination.h = height;
    
    blitInfo.load_op = SDL_GPU_LOADOP_CLEAR;
    blitInfo.clear_color.r = 0.0f;
    blitInfo.clear_color.g = 0.0f;
    blitInfo.clear_color.b = 0.0f;
    blitInfo.clear_color.a = 1.0f;
    blitInfo.filter = SDL_GPU_FILTER_LINEAR;
    
    SDL_BlitGPUTexture(cmdBuffer, &blitInfo);
    
    // Add color-shifted overlay blits for chromatic aberration effect
    float chromaOffset = sin(time * 3.0f) * 2.0f;
    
    // Red channel offset
    blitInfo.source.x = static_cast<Uint32>(wave1 + chromaOffset);
    blitInfo.load_op = SDL_GPU_LOADOP_LOAD;
    blitInfo.filter = SDL_GPU_FILTER_NEAREST;
    SDL_BlitGPUTexture(cmdBuffer, &blitInfo);
}

} // namespace Runa

