// File: src/Graphics/SpriteBatch.cpp

#include "SpriteBatch.h"
#include "Renderer.h"
#include <iostream>

namespace Runa {

SpriteBatch::SpriteBatch(Renderer& renderer)
    : m_renderer(renderer) {
}

void SpriteBatch::begin() {
    if (m_inBatch) {
        std::cerr << "SpriteBatch::begin() called while already in batch!" << std::endl;
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
        std::cerr << "SpriteBatch::draw() called outside of begin/end!" << std::endl;
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
        std::cerr << "SpriteBatch::end() called without begin!" << std::endl;
        return;
    }

    // For now, we'll just log the draw calls
    // Full GPU rendering implementation would go here
    std::cout << "SpriteBatch: Rendering " << m_drawCalls.size() << " sprites" << std::endl;

    // TODO: Implement actual GPU rendering
    // This would involve:
    // 1. Creating a vertex buffer with quad data
    // 2. Setting up graphics pipeline with texture sampling
    // 3. Binding textures and drawing batches
    // 4. Using the shader system for sprite rendering

    m_drawCalls.clear();
    m_inBatch = false;
}

} // namespace Runa
