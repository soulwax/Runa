// File: src/Graphics/SpriteBatch.cpp

#include "../runapch.h"
#include "SpriteBatch.h"
#include "Renderer.h"
#include "Shader.h"
#include "Core/Log.h"

namespace Runa {

SpriteBatch::SpriteBatch(Renderer& renderer)
    : m_renderer(renderer) {
    initializeShader();
}

void SpriteBatch::initializeShader() {
    try {
        m_shader = m_renderer.createShader("Resources/shaders/compiled/sprite.vert.spv", "Resources/shaders/compiled/sprite.frag.spv");
        if (!m_shader || !m_shader->isValid()) {
            LOG_WARN("Failed to load sprite shader, falling back to basic shader");
            m_shader = m_renderer.createShader("Resources/shaders/compiled/basic.vert.spv", "Resources/shaders/compiled/basic.frag.spv");
        }
    } catch (const std::exception& e) {
        LOG_ERROR("Error initializing sprite shader: {}", e.what());
    }
}

void SpriteBatch::begin() {
    if (m_inBatch) {
        LOG_WARN("SpriteBatch::begin() called while already in batch!");
        return;
    }
    m_inBatch = true;
    m_drawCalls.clear();
}

void SpriteBatch::draw(const Texture& texture, int x, int y, int srcX, int srcY,
                       int srcWidth, int srcHeight,
                       float r, float g, float b, float a,
                       float scaleX, float scaleY) {
    if (!m_inBatch) {
        LOG_WARN("SpriteBatch::draw() called outside of begin/end!");
        return;
    }

    SpriteDrawCall call;
    call.texture = &texture;
    call.x = x;
    call.y = y;
    call.srcX = srcX;
    call.srcY = srcY;
    call.srcWidth = srcWidth;
    call.srcHeight = srcHeight;
    call.r = r;
    call.g = g;
    call.b = b;
    call.a = a;
    call.scaleX = scaleX;
    call.scaleY = scaleY;

    m_drawCalls.push_back(call);
}

void SpriteBatch::draw(const Texture& texture, int x, int y, const SpriteFrame& frame,
                       float r, float g, float b, float a,
                       float scaleX, float scaleY) {
    draw(texture, x, y, frame.x, frame.y, frame.width, frame.height,
         r, g, b, a, scaleX, scaleY);
}

void SpriteBatch::draw(const Texture& texture, int x, int y,
                       float r, float g, float b, float a,
                       float scaleX, float scaleY) {
    draw(texture, x, y, 0, 0, texture.getWidth(), texture.getHeight(),
         r, g, b, a, scaleX, scaleY);
}

void SpriteBatch::end() {
    if (!m_inBatch) {
        LOG_WARN("SpriteBatch::end() called without begin!");
        return;
    }

    if (m_drawCalls.empty()) {
        m_drawCalls.clear();
        m_inBatch = false;
        return;
    }

    SDL_GPUDevice* device = m_renderer.getDevice();
    if (!device) {
        LOG_ERROR("Invalid GPU device in SpriteBatch::end()");
        m_drawCalls.clear();
        m_inBatch = false;
        return;
    }

    // Acquire command buffer first - we need it to acquire the swapchain
    SDL_GPUCommandBuffer* cmdBuffer = m_renderer.acquireCommandBuffer();
    if (!cmdBuffer) {
        LOG_ERROR("Failed to acquire command buffer: {}", SDL_GetError());
        m_drawCalls.clear();
        m_inBatch = false;
        return;
    }

    // Acquire swapchain texture on the same command buffer that will use it
    SDL_GPUTexture* swapchainTexture = nullptr;
    if (!SDL_AcquireGPUSwapchainTexture(cmdBuffer, m_renderer.getWindow().getHandle(), &swapchainTexture, nullptr, nullptr)) {
        LOG_ERROR("Failed to acquire swapchain texture: {}", SDL_GetError());
        SDL_CancelGPUCommandBuffer(cmdBuffer);
        m_drawCalls.clear();
        m_inBatch = false;
        return;
    }

    if (!swapchainTexture) {
        // Swapchain not available yet (too many frames in flight)
        // This is not an error, just skip this frame
        SDL_CancelGPUCommandBuffer(cmdBuffer);
        m_drawCalls.clear();
        m_inBatch = false;
        return;
    }

    if (m_renderer.needsClear()) {
        SDL_GPUColorTargetInfo colorTarget{};
        colorTarget.texture = swapchainTexture;
        auto clearColor = m_renderer.getClearColor();
        colorTarget.clear_color.r = clearColor.r;
        colorTarget.clear_color.g = clearColor.g;
        colorTarget.clear_color.b = clearColor.b;
        colorTarget.clear_color.a = clearColor.a;
        colorTarget.load_op = SDL_GPU_LOADOP_CLEAR;
        colorTarget.store_op = SDL_GPU_STOREOP_STORE;
        
        SDL_GPURenderPass* renderPass = SDL_BeginGPURenderPass(cmdBuffer, &colorTarget, 1, nullptr);
        if (!renderPass) {
            LOG_ERROR("Failed to begin render pass: {}", SDL_GetError());
            SDL_CancelGPUCommandBuffer(cmdBuffer);
            m_drawCalls.clear();
            m_inBatch = false;
            return;
        }
        SDL_EndGPURenderPass(renderPass);
        m_renderer.clearApplied();
    }

    // Render each sprite using SDL_BlitGPUTexture (called outside render pass)
    for (const auto& call : m_drawCalls) {
        renderSprite(call, cmdBuffer, nullptr, swapchainTexture);
    }

    SDL_SubmitGPUCommandBuffer(cmdBuffer);

    // Throttle logging to once per second
    static auto lastLogTime = std::chrono::steady_clock::now();
    static size_t totalSprites = 0;
    static size_t batchCount = 0;
    
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - lastLogTime).count();
    
    totalSprites += m_drawCalls.size();
    batchCount++;
    
    if (elapsed >= 1) {
        LOG_TRACE("SpriteBatch: Rendered {} sprites ({} batches)", totalSprites, batchCount);
        totalSprites = 0;
        batchCount = 0;
        lastLogTime = now;
    }
    
    m_drawCalls.clear();
    m_inBatch = false;
}

void SpriteBatch::renderSprite(const SpriteDrawCall& call, SDL_GPUCommandBuffer* cmdBuffer, SDL_GPURenderPass* renderPass, SDL_GPUTexture* swapchainTexture) {
    if (!call.texture || !call.texture->isValid() || !swapchainTexture) {
        return;
    }

    // Use SDL_BlitGPUTexture for simple sprite rendering
    // This must be called OUTSIDE a render pass
    
    SDL_GPUBlitInfo blitInfo{};
    SDL_zero(blitInfo);
    
    // Source texture region
    blitInfo.source.texture = call.texture->getHandle();
    blitInfo.source.x = call.srcX;
    blitInfo.source.y = call.srcY;
    blitInfo.source.w = call.srcWidth;
    blitInfo.source.h = call.srcHeight;
    blitInfo.source.mip_level = 0;
    blitInfo.source.layer_or_depth_plane = 0;
    
    // Destination region (on the swapchain)
    blitInfo.destination.texture = swapchainTexture;
    blitInfo.destination.x = call.x;
    blitInfo.destination.y = call.y;
    blitInfo.destination.w = static_cast<Uint32>(call.srcWidth * call.scaleX);
    blitInfo.destination.h = static_cast<Uint32>(call.srcHeight * call.scaleY);
    blitInfo.destination.mip_level = 0;
    blitInfo.destination.layer_or_depth_plane = 0;
    
    // Blit settings
    blitInfo.load_op = SDL_GPU_LOADOP_LOAD;  // Load existing content (blend with what's there)
    blitInfo.filter = SDL_GPU_FILTER_NEAREST;  // Pixel-perfect for sprites
    blitInfo.flip_mode = SDL_FLIP_NONE;
    blitInfo.cycle = false;
    // Note: SDL_BlitGPUTexture automatically handles alpha blending if texture has alpha channel
    
    // Perform the blit (outside render pass)
    SDL_BlitGPUTexture(cmdBuffer, &blitInfo);
}

} // namespace Runa
