// File: src/Graphics/SpriteBatch.cpp

#include "SpriteBatch.h"
#include "../runapch.h"
#include "Core/Log.h"
#include "Renderer.h"

namespace Runa {

float SpriteBatch::s_pixelScale = PixelScale::getScale();

SpriteBatch::SpriteBatch(Renderer &renderer) : m_renderer(renderer) {
    // Sync with PixelScale system
    s_pixelScale = PixelScale::getScale();
    LOG_INFO("SpriteBatch created (using Vulkan2D internal batching, pixel scale: {})", s_pixelScale);
}

SpriteBatch::~SpriteBatch() {

}

void SpriteBatch::begin() {
    if (m_inBatch) {
        LOG_WARN("SpriteBatch::begin() called while already in batch!");
        return;
    }
    m_inBatch = true;


}

void SpriteBatch::draw(const Texture &texture, int x, int y, int srcX, int srcY,
                       int srcWidth, int srcHeight, float r, float g, float b,
                       float a, float scaleX, float scaleY, bool flipX, bool flipY) {
    if (!m_inBatch) {
        LOG_WARN("SpriteBatch::draw() called outside of begin/end!");
        return;
    }

    if (!texture.isValid()) {
        return;
    }

    // Apply pixel scale
    float finalScaleX = scaleX * s_pixelScale;
    float finalScaleY = scaleY * s_pixelScale;

    float drawX = static_cast<float>(x);
    float drawY = static_cast<float>(y);

    // Handle flip by shifting position and negating scale
    // With negative scale, sprite draws to the LEFT of drawX instead of right
    // To keep the same visual position, shift drawX right by the rendered width
    if (flipX) {
        drawX += srcWidth * s_pixelScale * scaleX;  // Shift by rendered width
        finalScaleX = -finalScaleX;
    }

    if (flipY) {
        drawY += srcHeight * s_pixelScale * scaleY;  // Shift by rendered height
        finalScaleY = -finalScaleY;
    }

    vk2dRendererDrawTexture(
        texture.getHandle(),
        drawX,
        drawY,
        finalScaleX,
        finalScaleY,
        0.0f,
        0.0f,  // Keep origin at top-left
        0.0f,
        static_cast<float>(srcX),
        static_cast<float>(srcY),
        static_cast<float>(srcWidth),
        static_cast<float>(srcHeight)
    );


    if (r != 1.0f || g != 1.0f || b != 1.0f || a != 1.0f) {




        static bool warnedOnce = false;
        if (!warnedOnce) {
            LOG_WARN("Per-sprite color tinting not yet implemented with Vulkan2D");
            warnedOnce = true;
        }
    }
}

void SpriteBatch::draw(const Texture &texture, int x, int y,
                       const SpriteFrame &frame, float r, float g, float b,
                       float a, float scaleX, float scaleY, bool flipX, bool flipY) {
    draw(texture, x, y, frame.x, frame.y, frame.width, frame.height, r, g, b, a,
         scaleX, scaleY, flipX, flipY);
}

void SpriteBatch::draw(const Texture &texture, int x, int y, float r, float g,
                       float b, float a, float scaleX, float scaleY, bool flipX, bool flipY) {
    draw(texture, x, y, 0, 0, texture.getWidth(), texture.getHeight(), r, g, b, a,
         scaleX, scaleY, flipX, flipY);
}

void SpriteBatch::end() {
    if (!m_inBatch) {
        LOG_WARN("SpriteBatch::end() called without begin!");
        return;
    }

    m_inBatch = false;



}

}
