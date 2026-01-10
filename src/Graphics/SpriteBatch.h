// File: src/Graphics/SpriteBatch.h

#ifndef RUNA_GRAPHICS_SPRITEBATCH_H
#define RUNA_GRAPHICS_SPRITEBATCH_H

#include "RunaAPI.h"
#include "Texture.h"
#include "SpriteSheet.h"
#include "PixelScale.h"
#include <VK2D/VK2D.h>

namespace Runa
{

    class Renderer;

    
    class RUNA_API SpriteBatch
    {
    public:
        
        struct Vertex
        {
            float x, y;       
            float u, v;       
            float r, g, b, a; 
        };

        explicit SpriteBatch(Renderer &renderer);
        ~SpriteBatch();

        
        static void setPixelScale(float scale) { 
            s_pixelScale = scale; 
            PixelScale::setScale(scale);
        }
        static float getPixelScale() { return s_pixelScale; }
        
        void begin();

        
        void draw(const Texture &texture, int x, int y, int srcX, int srcY,
                  int srcWidth, int srcHeight,
                  float r = 1.0f, float g = 1.0f, float b = 1.0f, float a = 1.0f,
                  float scaleX = 1.0f, float scaleY = 1.0f,
                  bool flipX = false, bool flipY = false);

        
        void draw(const Texture &texture, int x, int y, const SpriteFrame &frame,
                  float r = 1.0f, float g = 1.0f, float b = 1.0f, float a = 1.0f,
                  float scaleX = 1.0f, float scaleY = 1.0f,
                  bool flipX = false, bool flipY = false);

        
        void draw(const Texture &texture, int x, int y,
                  float r = 1.0f, float g = 1.0f, float b = 1.0f, float a = 1.0f,
                  float scaleX = 1.0f, float scaleY = 1.0f,
                  bool flipX = false, bool flipY = false);

        
        void end();

    private:
        Renderer &m_renderer;
        bool m_inBatch = false;
        static float s_pixelScale;
        
    };

} 

#endif 
