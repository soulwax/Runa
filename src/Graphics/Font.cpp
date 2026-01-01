// File: src/Graphics/Font.cpp

#include "../runapch.h"
#include "Font.h"
#include "Renderer.h"
#include "Core/Log.h"

namespace Runa {

Font::Font(Renderer& renderer, const std::string& fontPath, int fontSize)
    : m_renderer(renderer), m_fontSize(fontSize) {
    
    // Initialize TTF if not already initialized
    if (!TTF_WasInit() && !TTF_Init()) {
        LOG_ERROR("Failed to initialize SDL_ttf: {}", SDL_GetError());
        return;
    }

    // Load font
    LOG_DEBUG("Attempting to open font: {} at size {}", fontPath, fontSize);
    m_font = TTF_OpenFont(fontPath.c_str(), static_cast<float>(fontSize));
    if (!m_font) {
        LOG_ERROR("Failed to load font '{}': {}", fontPath, SDL_GetError());
        LOG_ERROR("TTF_OpenFont failed - check if font file exists and is valid");
        return;
    }

    LOG_INFO("Font loaded successfully: {} (size: {})", fontPath, fontSize);
    
    // Test if font can render a simple character
    SDL_Surface* testSurface = TTF_RenderText_Blended(m_font, "A", 1, {255, 255, 255, 255});
    if (testSurface) {
        LOG_DEBUG("Font test render successful: {}x{}", testSurface->w, testSurface->h);
        SDL_DestroySurface(testSurface);
    } else {
        LOG_WARN("Font test render failed: {}", SDL_GetError());
    }
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

    // Render text to surface with alpha blending (transparent background)
    SDL_Surface* textSurface = TTF_RenderText_Blended(m_font, text.c_str(), text.size(), color);
    if (!textSurface) {
        LOG_ERROR("Failed to render text '{}': {}", text, SDL_GetError());
        return nullptr;
    }

    // Check if surface has valid dimensions
    if (textSurface->w == 0 || textSurface->h == 0) {
        LOG_ERROR("Text surface has invalid dimensions: {}x{}", textSurface->w, textSurface->h);
        SDL_DestroySurface(textSurface);
        return nullptr;
    }

    // Convert to RGBA32 to ensure proper alpha channel handling
    SDL_Surface* rgbaSurface = SDL_ConvertSurface(textSurface, SDL_PIXELFORMAT_RGBA32);
    SDL_DestroySurface(textSurface);
    if (!rgbaSurface) {
        LOG_ERROR("Failed to convert text surface to RGBA format: {}", SDL_GetError());
        return nullptr;
    }

    // TTF_RenderText_Blended creates text with transparent background already
    // No manual processing needed - the surface should be correct as-is

    // Create texture from pixel data
    auto texture = std::make_unique<Texture>(m_renderer, rgbaSurface->w, rgbaSurface->h, rgbaSurface->pixels);
    
    SDL_DestroySurface(rgbaSurface);
    return texture;
}

} // namespace Runa
