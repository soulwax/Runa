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

    LOG_DEBUG("Rendering text: '{}' with color R={}, G={}, B={}, A={}", 
              text, color.r, color.g, color.b, color.a);

    // Render text to surface with alpha blending (transparent background)
    // Note: TTF_RenderText_Blended should create transparent background, but may need manual processing
    SDL_Surface* textSurface = TTF_RenderText_Blended(m_font, text.c_str(), text.size(), color);
    if (!textSurface) {
        LOG_ERROR("Failed to render text surface for '{}': {}", text, SDL_GetError());
        return nullptr;
    }
    
    // Check if surface has valid dimensions
    if (textSurface->w == 0 || textSurface->h == 0) {
        LOG_ERROR("Text surface has invalid dimensions: {}x{}", textSurface->w, textSurface->h);
        SDL_DestroySurface(textSurface);
        return nullptr;
    }

    LOG_DEBUG("Text surface created: {}x{}, format: {}", textSurface->w, textSurface->h,
              textSurface->format ? SDL_GetPixelFormatName(textSurface->format) : "unknown");
    
    // Check the original surface format
    LOG_DEBUG("Original surface format: {}, has alpha: {}", 
              SDL_GetPixelFormatName(textSurface->format),
              SDL_ISPIXELFORMAT_ALPHA(textSurface->format));

    // Always convert to RGBA32 to ensure proper alpha channel
    // This ensures the alpha channel is preserved correctly
    SDL_Surface* rgbaSurface = SDL_ConvertSurface(textSurface, SDL_PIXELFORMAT_RGBA32);
    SDL_DestroySurface(textSurface);
    if (!rgbaSurface) {
        LOG_ERROR("Failed to convert text surface to RGBA format: {}", SDL_GetError());
        return nullptr;
    }
    
    LOG_DEBUG("Converted to RGBA32: {}x{}, pitch: {}, format: {}", 
              rgbaSurface->w, rgbaSurface->h, rgbaSurface->pitch,
              rgbaSurface->format ? SDL_GetPixelFormatName(rgbaSurface->format) : "unknown");
    
    // Manually process the surface to ensure white background pixels are transparent
    // TTF_RenderText_Blended may create a surface with white background instead of transparent
    if (rgbaSurface->pixels && rgbaSurface->w > 0 && rgbaSurface->h > 0) {
        Uint8* pixels = static_cast<Uint8*>(rgbaSurface->pixels);
        int pixelCount = rgbaSurface->w * rgbaSurface->h;
        int transparentCount = 0;
        int whiteToTransparentCount = 0;
        
        // Process each pixel: if it's white (or near-white), make it transparent
        for (int i = 0; i < pixelCount; i++) {
            int idx = i * 4;
            Uint8 r = pixels[idx];
            Uint8 g = pixels[idx + 1];
            Uint8 b = pixels[idx + 2];
            Uint8 a = pixels[idx + 3];
            
            // If pixel is already transparent, skip
            if (a < 10) {
                transparentCount++;
                continue;
            }
            
            // If pixel is pure white with full alpha, it's likely background
            // Text pixels should have some variation or be anti-aliased (not pure 255,255,255,255)
            // Only convert pure white (255,255,255) with high alpha to transparent
            if (r == 255 && g == 255 && b == 255 && a == 255) {
                // Make transparent by setting alpha to 0
                pixels[idx + 3] = 0;
                whiteToTransparentCount++;
            }
        }
        
        LOG_DEBUG("Processed surface: {} already transparent, {} white->transparent", 
                  transparentCount, whiteToTransparentCount);
    }
    
    // DEBUG: Save surface to file to visually inspect what was rendered
    static int debugSaveCount = 0;
    if (debugSaveCount < 3) {  // Only save first 3 renders
        std::string debugPath = "debug_font_render_" + std::to_string(debugSaveCount++) + ".png";
        if (IMG_SavePNG(rgbaSurface, debugPath.c_str()) == 0) {
            LOG_DEBUG("Saved debug font render to: {}", debugPath);
        } else {
            LOG_WARN("Failed to save debug font render: {}", SDL_GetError());
        }
    }
    
    // Sample pixels to verify text is actually rendered (debug)
    if (rgbaSurface->pixels && rgbaSurface->w > 0 && rgbaSurface->h > 0) {
        Uint8* pixels = static_cast<Uint8*>(rgbaSurface->pixels);
        
        // Check multiple pixels to see if text is present
        int sampleCount = 0;
        int nonWhiteCount = 0;
        int transparentCount = 0;
        
        // Sample a grid of pixels
        for (int y = 0; y < rgbaSurface->h; y += rgbaSurface->h / 4) {
            for (int x = 0; x < rgbaSurface->w; x += rgbaSurface->w / 4) {
                int idx = (y * rgbaSurface->pitch) + (x * 4);
                if (idx + 3 < rgbaSurface->pitch * rgbaSurface->h) {
                    Uint8 r = pixels[idx];
                    Uint8 g = pixels[idx + 1];
                    Uint8 b = pixels[idx + 2];
                    Uint8 a = pixels[idx + 3];
                    
                    sampleCount++;
                    if (a < 10) {  // Transparent
                        transparentCount++;
                    } else if (r < 250 || g < 250 || b < 250) {  // Not white
                        nonWhiteCount++;
                    }
                }
            }
        }
        
        LOG_DEBUG("Surface pixel analysis: {} samples, {} transparent, {} non-white", 
                  sampleCount, transparentCount, nonWhiteCount);
        
        // Check center pixel specifically
        int centerX = rgbaSurface->w / 2;
        int centerY = rgbaSurface->h / 2;
        int centerIdx = (centerY * rgbaSurface->pitch) + (centerX * 4);
        if (centerIdx + 3 < rgbaSurface->pitch * rgbaSurface->h) {
            Uint8 r = pixels[centerIdx];
            Uint8 g = pixels[centerIdx + 1];
            Uint8 b = pixels[centerIdx + 2];
            Uint8 a = pixels[centerIdx + 3];
            LOG_DEBUG("Center pixel: R={}, G={}, B={}, A={}", r, g, b, a);
        }
        
        // If all pixels are white/opaque, text rendering likely failed
        if (nonWhiteCount == 0 && transparentCount < sampleCount / 2) {
            LOG_WARN("Warning: Surface appears to be all white - text may not have rendered correctly");
        }
    }

    // Create texture from pixel data
    auto texture = std::make_unique<Texture>(m_renderer, rgbaSurface->w, rgbaSurface->h, rgbaSurface->pixels);
    
    SDL_DestroySurface(rgbaSurface);
    return texture;
}

} // namespace Runa

