// File: src/Graphics/Texture.h

#pragma once

#include "../RunaAPI.h"
#include <SDL3/SDL.h>
#include <string>

namespace Runa
{

    class Renderer;

    class RUNA_API Texture
    {
    public:
        Texture(Renderer &renderer, const std::string &path);
        Texture(Renderer &renderer, int width, int height, const void *pixelData);
        ~Texture();

        // Disable copying
        Texture(const Texture &) = delete;
        Texture &operator=(const Texture &) = delete;

        // Enable moving
        Texture(Texture &&other) noexcept;
        Texture &operator=(Texture &&other) noexcept;

        SDL_GPUTexture *getHandle() const { return m_texture; }
        int getWidth() const { return m_width; }
        int getHeight() const { return m_height; }
        bool isValid() const { return m_texture != nullptr; }

        // Get texture region info
        struct Rect
        {
            int x, y, w, h;
        };

    private:
        SDL_GPUDevice *m_device = nullptr;
        SDL_GPUTexture *m_texture = nullptr;
        int m_width = 0;
        int m_height = 0;

        void loadFromFile(const std::string &path);
        void createFromPixels(int width, int height, const void *pixelData);
    };

} // namespace Runa
