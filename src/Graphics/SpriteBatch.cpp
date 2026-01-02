// File: src/Graphics/SpriteBatch.cpp

#include "SpriteBatch.h"
#include "../runapch.h"
#include "Core/Log.h"
#include "Renderer.h"

namespace Runa {

SpriteBatch::SpriteBatch(Renderer &renderer) : m_renderer(renderer) {
    // No initialization needed - Vulkan2D renderer already set up
    LOG_INFO("SpriteBatch created (using Vulkan2D internal batching)");
}

SpriteBatch::~SpriteBatch() {
    // No cleanup needed - no GPU resources owned by SpriteBatch
}

void SpriteBatch::begin() {
    if (m_inBatch) {
        LOG_WARN("SpriteBatch::begin() called while already in batch!");
        return;
    }
    m_inBatch = true;
    // Vulkan2D doesn't require explicit batch begin
    // vk2dRendererStartFrame() already called by Renderer::beginFrame()
}

void SpriteBatch::draw(const Texture &texture, int x, int y, int srcX, int srcY,
                       int srcWidth, int srcHeight, float r, float g, float b,
                       float a, float scaleX, float scaleY) {
    if (!m_inBatch) {
        LOG_WARN("SpriteBatch::draw() called outside of begin/end!");
        return;
    }

    if (!texture.isValid()) {
        return;
    }

    // Vulkan2D draw texture with extended parameters
    // vk2dRendererDrawTexture signature:
    // (VK2DTexture tex, float x, float y, float xscale, float yscale, float rot,
    //  float originX, float originY, float xInTex, float yInTex, float texWidth, float texHeight)
    vk2dRendererDrawTexture(
        texture.getHandle(),
        static_cast<float>(x),
        static_cast<float>(y),
        scaleX,
        scaleY,
        0.0f,  // rotation
        0.0f,  // originX (top-left)
        0.0f,  // originY (top-left)
        static_cast<float>(srcX),
        static_cast<float>(srcY),
        static_cast<float>(srcWidth),
        static_cast<float>(srcHeight)
    );

    // Apply color tint via renderer color mod if not white
    if (r != 1.0f || g != 1.0f || b != 1.0f || a != 1.0f) {
        // Vulkan2D uses color mod via vk2dRendererSetColourMod
        // This affects all subsequent draws until changed
        // For per-sprite tinting, we'd need to set it before each draw
        // For now, log a warning if non-white tint is used
        static bool warnedOnce = false;
        if (!warnedOnce) {
            LOG_WARN("Per-sprite color tinting not yet implemented with Vulkan2D");
            warnedOnce = true;
        }
    }
}

void SpriteBatch::draw(const Texture &texture, int x, int y,
                       const SpriteFrame &frame, float r, float g, float b,
                       float a, float scaleX, float scaleY) {
    draw(texture, x, y, frame.x, frame.y, frame.width, frame.height, r, g, b, a,
         scaleX, scaleY);
}

void SpriteBatch::draw(const Texture &texture, int x, int y, float r, float g,
                       float b, float a, float scaleX, float scaleY) {
    draw(texture, x, y, 0, 0, texture.getWidth(), texture.getHeight(), r, g, b, a,
         scaleX, scaleY);
}

void SpriteBatch::end() {
    if (!m_inBatch) {
        LOG_WARN("SpriteBatch::end() called without begin!");
        return;
    }

    m_inBatch = false;

    // No explicit flush needed - Vulkan2D batches internally
    // All draw calls are automatically batched and submitted by vk2dRendererEndFrame()
}

} // namespace Runa
