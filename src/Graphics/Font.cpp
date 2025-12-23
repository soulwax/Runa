// File: src/Graphics/Font.cpp

#include "Font.h"
#include "Renderer.h"
#include "Core/Log.h"
#include <SDL3_ttf/SDL_ttf.h>
#include <sstream>

namespace Runa {

Font::Font(Renderer& renderer, const std::string& fontPath, int fontSize)
    : m_renderer(renderer), m_fontSize(fontSize) {
    
    // Initialize TTF if not already initialized
    if (!TTF_WasInit() && TTF_Init() != 0) {
        LOG_ERROR("Failed to initialize SDL_ttf: {}", TTF_GetError());
        return;
    }

    // Load font
    m_font = TTF_OpenFont(fontPath.c_str(), fontSize);
    if (!m_font) {
        LOG_ERROR("Failed to load font '{}': {}", fontPath, TTF_GetError());
        return;
    }

    LOG_INFO("Font loaded: {} (size: {})", fontPath, fontSize);
}

Font::~Font() {
    if (m_font) {
        TTF_CloseFont(m_font);
        m_font = nullptr;
    }
}

Font::Font(Font&& other) noexcept
    : m_renderer(other.m_renderer),
      m_font(other.m_font),
      m_fontSize(other.m_fontSize) {
    other.m_font = nullptr;
}

Font& Font::operator=(Font&& other) noexcept {
    if (this != &other) {
        if (m_font) {
            TTF_CloseFont(m_font);
        }
        m_font = other.m_font;
        m_fontSize = other.m_fontSize;
        other.m_font = nullptr;
    }
    return *this;
}

std::unique_ptr<Texture> Font::renderText(const std::string& text, SDL_Color color) {
    if (!m_font) {
        LOG_ERROR("Cannot render text: font not loaded");
        return nullptr;
    }

    if (text.empty()) {
        // Return a small empty texture
        return std::make_unique<Texture>(m_renderer, 1, 1, nullptr);
    }

    // Render text to surface
    SDL_Surface* textSurface = TTF_RenderText_Solid(m_font, text.c_str(), color);
    if (!textSurface) {
        LOG_ERROR("Failed to render text surface: {}", TTF_GetError());
        return nullptr;
    }

    // Convert surface to RGBA32 format if needed
    SDL_Surface* rgbaSurface = textSurface;
    if (textSurface->format->format != SDL_PIXELFORMAT_RGBA32) {
        rgbaSurface = SDL_ConvertSurface(textSurface, SDL_PIXELFORMAT_RGBA32);
        SDL_DestroySurface(textSurface);
        if (!rgbaSurface) {
            LOG_ERROR("Failed to convert text surface to RGBA format: {}", SDL_GetError());
            return nullptr;
        }
    }

    // Create texture from pixel data
    auto texture = std::make_unique<Texture>(m_renderer, rgbaSurface->w, rgbaSurface->h, rgbaSurface->pixels);
    
    SDL_DestroySurface(rgbaSurface);
    return texture;
}

} // namespace Runa

