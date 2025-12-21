// File: src/Graphics/SpriteBatch.h

#pragma once

#include "Texture.h"
#include "SpriteSheet.h"
#include <SDL3/SDL.h>
#include <vector>

namespace Runa {

class Renderer;

// Simple sprite batch renderer for 2D sprites
class SpriteBatch {
public:
    struct Vertex {
        float x, y;       // Position
        float u, v;       // Texture coordinates
        float r, g, b, a; // Color
    };

    struct SpriteDrawCall {
        const Texture* texture;
        int x, y;                    // Screen position
        int srcX, srcY;              // Source position in texture
        int srcWidth, srcHeight;     // Source dimensions
        float r, g, b, a;            // Tint color
        float scaleX, scaleY;        // Scale
    };

    explicit SpriteBatch(Renderer& renderer);
    ~SpriteBatch() = default;

    // Begin batch rendering
    void begin();

    // Draw a sprite region
    void draw(const Texture& texture, int x, int y, int srcX, int srcY,
              int srcWidth, int srcHeight,
              float r = 1.0f, float g = 1.0f, float b = 1.0f, float a = 1.0f,
              float scaleX = 1.0f, float scaleY = 1.0f);

    // Draw a sprite frame
    void draw(const Texture& texture, int x, int y, const SpriteFrame& frame,
              float r = 1.0f, float g = 1.0f, float b = 1.0f, float a = 1.0f,
              float scaleX = 1.0f, float scaleY = 1.0f);

    // Draw entire texture
    void draw(const Texture& texture, int x, int y,
              float r = 1.0f, float g = 1.0f, float b = 1.0f, float a = 1.0f,
              float scaleX = 1.0f, float scaleY = 1.0f);

    // End batch and flush to GPU
    void end();

private:
    Renderer& m_renderer;
    std::vector<SpriteDrawCall> m_drawCalls;
    bool m_inBatch = false;
};

} // namespace Runa
