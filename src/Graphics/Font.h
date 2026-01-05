// File: src/Graphics/Font.h

#ifndef RUNA_GRAPHICS_FONT_H
#define RUNA_GRAPHICS_FONT_H

#include "RunaAPI.h"
#include "Texture.h"
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <string>
#include <memory>

namespace Runa {

class Renderer;

class RUNA_API Font {
public:
    Font(Renderer& renderer, const std::string& fontPath, int fontSize);
    ~Font();

    // Disable copying
    Font(const Font&) = delete;
    Font& operator=(const Font&) = delete;

    // Enable moving
    Font(Font&& other) noexcept;
    Font& operator=(Font&& other) noexcept;

    // Render text to a texture
    std::unique_ptr<Texture> renderText(const std::string& text, SDL_Color color = {255, 255, 255, 255});
    
    bool isValid() const { return m_font != nullptr; }
    int getFontSize() const { return m_fontSize; }

private:
    Renderer& m_renderer;
    TTF_Font* m_font = nullptr;
    int m_fontSize = 0;
};

} // namespace Runa
#endif // RUNA_GRAPHICS_FONT_H





