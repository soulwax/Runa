

#include "SpriteBatch.h"
#include "../runapch.h"
#include "Core/Log.h"
#include "Renderer.h"

namespace Runa {

SpriteBatch::SpriteBatch(Renderer &renderer) : m_renderer(renderer) {

    LOG_INFO("SpriteBatch created (using Vulkan2D internal batching)");
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
                       float a, float scaleX, float scaleY) {
    if (!m_inBatch) {
        LOG_WARN("SpriteBatch::draw() called outside of begin/end!");
        return;
    }

    if (!texture.isValid()) {
        return;
    }





    vk2dRendererDrawTexture(
        texture.getHandle(),
        static_cast<float>(x),
        static_cast<float>(y),
        scaleX,
        scaleY,
        0.0f,
        0.0f,
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



}

}
