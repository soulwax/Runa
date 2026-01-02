// File: src/Graphics/SpriteBatch.h

#ifndef RUNA_GRAPHICS_SPRITEBATCH_H
#define RUNA_GRAPHICS_SPRITEBATCH_H

#include "RunaAPI.h"
#include "Texture.h"
#include "SpriteSheet.h"
#include <VK2D/VK2D.h>

namespace Runa
{

    class Renderer;

    // Simplified sprite batch renderer using Vulkan2D automatic batching
    class RUNA_API SpriteBatch
    {
    public:
        // Keep Vertex struct for API compatibility (not used internally)
        struct Vertex
        {
            float x, y;       // Position
            float u, v;       // Texture coordinates
            float r, g, b, a; // Color
        };

        explicit SpriteBatch(Renderer &renderer);
        ~SpriteBatch();

        // Begin batch rendering
        void begin();

        // Draw a sprite region
        void draw(const Texture &texture, int x, int y, int srcX, int srcY,
                  int srcWidth, int srcHeight,
                  float r = 1.0f, float g = 1.0f, float b = 1.0f, float a = 1.0f,
                  float scaleX = 1.0f, float scaleY = 1.0f);

        // Draw a sprite frame
        void draw(const Texture &texture, int x, int y, const SpriteFrame &frame,
                  float r = 1.0f, float g = 1.0f, float b = 1.0f, float a = 1.0f,
                  float scaleX = 1.0f, float scaleY = 1.0f);

        // Draw entire texture
        void draw(const Texture &texture, int x, int y,
                  float r = 1.0f, float g = 1.0f, float b = 1.0f, float a = 1.0f,
                  float scaleX = 1.0f, float scaleY = 1.0f);

        // End batch and flush to GPU
        void end();

    private:
        Renderer &m_renderer;
        bool m_inBatch = false;
        // Vulkan2D handles all batching internally - no buffers or pipelines needed
    };

} // namespace Runa

#endif // RUNA_GRAPHICS_SPRITEBATCH_H
