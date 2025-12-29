// File: src/Graphics/SpriteSheet.cpp

#include "runapch.h"
#include "SpriteSheet.h"
#include "Renderer.h"
#include "Core/Log.h"

namespace Runa {

SpriteSheet::SpriteSheet(Renderer& renderer, const std::string& texturePath) {
    m_texture = std::make_unique<Texture>(renderer, texturePath);
    if (!m_texture->isValid()) {
        throw std::runtime_error("Failed to load spritesheet texture: " + texturePath);
    }
    LOG_INFO("SpriteSheet created from: {}", texturePath);
}

void SpriteSheet::addSprite(const std::string& name, int x, int y, int width, int height) {
    Sprite sprite;
    sprite.name = name;
    sprite.loop = false;

    SpriteFrame frame;
    frame.x = x;
    frame.y = y;
    frame.width = width;
    frame.height = height;
    frame.duration = 0.0f;  // Single frame, no duration

    sprite.frames.push_back(frame);
    m_sprites[name] = sprite;

    LOG_DEBUG("Added sprite '{}' at ({},{}) size {}x{}", name, x, y, width, height);
}

void SpriteSheet::addAnimation(const std::string& name, int x, int y, int frameWidth, int frameHeight,
                                int frameCount, int columns, float frameDuration, bool loop) {
    Sprite sprite;
    sprite.name = name;
    sprite.loop = loop;

    int currentX = x;
    int currentY = y;
    int currentColumn = 0;

    for (int i = 0; i < frameCount; ++i) {
        SpriteFrame frame;
        frame.x = currentX;
        frame.y = currentY;
        frame.width = frameWidth;
        frame.height = frameHeight;
        frame.duration = frameDuration;

        sprite.frames.push_back(frame);

        // Move to next frame position
        currentColumn++;
        if (columns > 0 && currentColumn >= columns) {
            // Move to next row
            currentColumn = 0;
            currentX = x;
            currentY += frameHeight;
        } else {
            // Move to next column
            currentX += frameWidth;
        }
    }

    m_sprites[name] = sprite;

    LOG_DEBUG("Added animation '{}' with {} frames", name, frameCount);
}

void SpriteSheet::addSpriteFrames(const std::string& name, const std::vector<SpriteFrame>& frames, bool loop) {
    Sprite sprite;
    sprite.name = name;
    sprite.frames = frames;
    sprite.loop = loop;

    m_sprites[name] = sprite;

    LOG_DEBUG("Added sprite '{}' with {} frames", name, frames.size());
}

const Sprite* SpriteSheet::getSprite(const std::string& name) const {
    auto it = m_sprites.find(name);
    if (it != m_sprites.end()) {
        return &it->second;
    }
    return nullptr;
}

std::vector<std::string> SpriteSheet::getSpriteNames() const {
    std::vector<std::string> names;
    names.reserve(m_sprites.size());
    for (const auto& pair : m_sprites) {
        names.push_back(pair.first);
    }
    return names;
}

void SpriteSheet::createGrid(const std::string& baseName, int tileWidth, int tileHeight, int columns, int rows) {
    int texWidth = m_texture->getWidth();
    int texHeight = m_texture->getHeight();

    // Calculate columns and rows if not specified
    if (columns == 0) {
        columns = texWidth / tileWidth;
    }
    if (rows == 0) {
        rows = texHeight / tileHeight;
    }

    LOG_DEBUG("Creating grid: {}x{} tiles of size {}x{}", columns, rows, tileWidth, tileHeight);

    int index = 0;
    for (int row = 0; row < rows; ++row) {
        for (int col = 0; col < columns; ++col) {
            int x = col * tileWidth;
            int y = row * tileHeight;

            // Check if tile is within texture bounds
            if (x + tileWidth <= texWidth && y + tileHeight <= texHeight) {
                std::string tileName = baseName + "_" + std::to_string(index);
                addSprite(tileName, x, y, tileWidth, tileHeight);
                index++;
            }
        }
    }

    LOG_DEBUG("Created {} tiles from grid", index);
}

} // namespace Runa
