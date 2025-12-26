// File: src/Graphics/SpriteBatch.h

#pragma once

#include "../RunaAPI.h"
#include "Texture.h"
#include "SpriteSheet.h"
#include <SDL3/SDL.h>
#include <vector>
#include <memory>

namespace Runa
{

    class Renderer;
    class Shader;

    // Simple sprite batch renderer for 2D sprites
    class RUNA_API SpriteBatch
    {
    public:
        struct Vertex
        {
            float x, y;       // Position
            float u, v;       // Texture coordinates
            float r, g, b, a; // Color
        };

        struct SpriteDrawCall
        {
            const Texture *texture;
            int x, y;                // Screen position
            int srcX, srcY;          // Source position in texture
            int srcWidth, srcHeight; // Source dimensions
            float r, g, b, a;        // Tint color
            float scaleX, scaleY;    // Scale
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
        std::vector<SpriteDrawCall> m_drawCalls;
        bool m_inBatch = false;

        // Shader for sprite rendering
        std::shared_ptr<Shader> m_shader;

        // Graphics pipeline for alpha-blended rendering
        SDL_GPUGraphicsPipeline *m_pipeline = nullptr;
        SDL_GPUSampler *m_sampler = nullptr;

        // Vertex/index buffers for sprite quads
        SDL_GPUBuffer *m_vertexBuffer = nullptr;
        SDL_GPUBuffer *m_indexBuffer = nullptr;
        size_t m_maxSprites = 1000; // Max sprites per batch

        void initializeShader();
        void createPipeline();
        void createBuffers();
        void renderSprite(const SpriteDrawCall &call, SDL_GPUCommandBuffer *cmdBuffer, SDL_GPURenderPass *renderPass, SDL_GPUTexture *swapchainTexture);
    };

} // namespace Runa
