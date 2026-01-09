

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


    bool setTile(int x, int y, int tileIndex);


    int getTile(int x, int y) const;


    void clear(int fillTile = -1);


    void fillRect(int x, int y, int width, int height, int tileIndex);


    void render(SpriteBatch& batch, const SpriteSheet& tileset, const std::string& tileBaseName,
                int offsetX = 0, int offsetY = 0);


    int getWidth() const { return m_width; }
    int getHeight() const { return m_height; }
    int getTileSize() const { return m_tileSize; }


    void setSolidTile(int tileIndex, bool solid);
    bool isSolidTile(int tileIndex) const;


    void loadFromString(const std::string& data);

private:
    int m_width;
    int m_height;
    int m_tileSize;
    std::vector<int> m_tiles;
    std::unordered_set<int> m_solidTiles;

    int getIndex(int x, int y) const { return y * m_width + x; }
    bool isValid(int x, int y) const { return x >= 0 && x < m_width && y >= 0 && y < m_height; }
};

}

#endif
