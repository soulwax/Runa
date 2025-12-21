// File: src/Graphics/TileMap.cpp

#include "TileMap.h"
#include <sstream>
#include <iostream>

namespace Runa {

TileMap::TileMap(int width, int height, int tileSize)
    : m_width(width), m_height(height), m_tileSize(tileSize) {
    m_tiles.resize(width * height, -1);
    std::cout << "TileMap created: " << width << "x" << height
              << " tiles (" << tileSize << "px each)" << std::endl;
}

bool TileMap::setTile(int x, int y, int tileIndex) {
    if (!isValid(x, y)) return false;
    m_tiles[getIndex(x, y)] = tileIndex;
    return true;
}

int TileMap::getTile(int x, int y) const {
    if (!isValid(x, y)) return -1;
    return m_tiles[getIndex(x, y)];
}

void TileMap::clear(int fillTile) {
    std::fill(m_tiles.begin(), m_tiles.end(), fillTile);
}

void TileMap::fillRect(int x, int y, int width, int height, int tileIndex) {
    for (int dy = 0; dy < height; ++dy) {
        for (int dx = 0; dx < width; ++dx) {
            setTile(x + dx, y + dy, tileIndex);
        }
    }
}

void TileMap::render(SpriteBatch& batch, const SpriteSheet& tileset, const std::string& tileBaseName,
                     int offsetX, int offsetY) {
    for (int y = 0; y < m_height; ++y) {
        for (int x = 0; x < m_width; ++x) {
            int tileIndex = getTile(x, y);
            if (tileIndex < 0) continue;  // Skip empty tiles

            // Get sprite name (e.g., "plains_tile_0")
            std::string spriteName = tileBaseName + "_" + std::to_string(tileIndex);
            const Sprite* sprite = tileset.getSprite(spriteName);

            if (sprite && !sprite->frames.empty()) {
                const SpriteFrame& frame = sprite->frames[0];
                int screenX = offsetX + (x * m_tileSize);
                int screenY = offsetY + (y * m_tileSize);

                batch.draw(tileset.getTexture(), screenX, screenY, frame);
            }
        }
    }
}

void TileMap::loadFromString(const std::string& data) {
    std::istringstream stream(data);
    std::string line;
    int y = 0;

    while (std::getline(stream, line) && y < m_height) {
        std::istringstream lineStream(line);
        int x = 0;
        int tileIndex;

        while (lineStream >> tileIndex && x < m_width) {
            setTile(x, y, tileIndex);
            x++;
        }
        y++;
    }

    std::cout << "TileMap loaded from string (" << y << " rows parsed)" << std::endl;
}

} // namespace Runa
