// File: src/Graphics/TileMap.h

#ifndef RUNA_GRAPHICS_TILEMAP_H
#define RUNA_GRAPHICS_TILEMAP_H

#include "RunaAPI.h"
#include "SpriteBatch.h"
#include "SpriteSheet.h"
#include <vector>
#include <string>
#include <unordered_set>

namespace Runa {

class RUNA_API TileMap {
public:
    TileMap(int width, int height, int tileSize);
    ~TileMap() = default;

    // Set tile at position (returns false if out of bounds)
    bool setTile(int x, int y, int tileIndex);

    // Get tile at position (returns -1 if out of bounds or empty)
    int getTile(int x, int y) const;

    // Clear all tiles
    void clear(int fillTile = -1);

    // Fill a rectangular region
    void fillRect(int x, int y, int width, int height, int tileIndex);

    // Render the tilemap
    void render(SpriteBatch& batch, const SpriteSheet& tileset, const std::string& tileBaseName,
                int offsetX = 0, int offsetY = 0);

    // Getters
    int getWidth() const { return m_width; }
    int getHeight() const { return m_height; }
    int getTileSize() const { return m_tileSize; }

    // Collision: mark tiles as solid
    void setSolidTile(int tileIndex, bool solid);
    bool isSolidTile(int tileIndex) const;

    // Load from simple text format (for prototyping)
    void loadFromString(const std::string& data);

private:
    int m_width;
    int m_height;
    int m_tileSize;
    std::vector<int> m_tiles;  // Tile indices (-1 = empty)
    std::unordered_set<int> m_solidTiles;  // Set of solid tile indices

    int getIndex(int x, int y) const { return y * m_width + x; }
    bool isValid(int x, int y) const { return x >= 0 && x < m_width && y >= 0 && y < m_height; }
};

} // namespace Runa

#endif // RUNA_GRAPHICS_TILEMAP_H
