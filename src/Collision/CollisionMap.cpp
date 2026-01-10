#include "CollisionMap.h"
#include "../Core/Log.h"
#include <algorithm>
#include <cmath>

namespace Runa {

CollisionMap::CollisionMap(int worldWidth, int worldHeight, int tileSize)
    : m_worldWidth(worldWidth)
    , m_worldHeight(worldHeight)
    , m_tileSize(tileSize) {
    // Initialize spatial grid
    m_gridWidth = (worldWidth + m_gridCellSize - 1) / m_gridCellSize;
    m_gridHeight = (worldHeight + m_gridCellSize - 1) / m_gridCellSize;
    m_spatialGrid.resize(static_cast<size_t>(m_gridWidth) * m_gridHeight);
}

int CollisionMap::addTileDefinition(const TileDefinition& def) {
    int index = static_cast<int>(m_tileDefinitions.size());
    m_tileDefinitions.push_back(def);
    
    if (!def.name.empty()) {
        m_tileNameToIndex[def.name] = index;
    }
    
    return index;
}

TileDefinition* CollisionMap::getTileDefinition(int index) {
    if (index < 0 || index >= static_cast<int>(m_tileDefinitions.size())) {
        return nullptr;
    }
    return &m_tileDefinitions[index];
}

TileDefinition* CollisionMap::getTileDefinition(const std::string& name) {
    auto it = m_tileNameToIndex.find(name);
    if (it != m_tileNameToIndex.end()) {
        return &m_tileDefinitions[it->second];
    }
    return nullptr;
}

void CollisionMap::placeTile(int tileDefIndex, int worldX, int worldY, int width, int height) {
    if (tileDefIndex < 0 || tileDefIndex >= static_cast<int>(m_tileDefinitions.size())) {
        LOG_WARN("CollisionMap::placeTile: Invalid tile definition index {}", tileDefIndex);
        return;
    }
    
    PlacedTile tile;
    tile.tileDefIndex = tileDefIndex;
    tile.worldX = worldX;
    tile.worldY = worldY;
    tile.width = width;
    tile.height = height;
    
    // Copy interaction data if the tile has one
    auto& def = m_tileDefinitions[tileDefIndex];
    if (def.interaction.type != InteractionType::None) {
        // We need to store runtime interaction state separately
        // For simplicity, we'll point to the definition's interaction
        // In a real implementation, you might want to clone it for per-tile state
        tile.interaction = &def.interaction;
    }
    
    int tileIndex = static_cast<int>(m_placedTiles.size());
    m_placedTiles.push_back(tile);
    
    // Add to spatial grid
    std::vector<int> cells = getGridCellsForRegion(
        static_cast<float>(worldX), static_cast<float>(worldY),
        static_cast<float>(width), static_cast<float>(height));
    
    for (int cellIndex : cells) {
        if (cellIndex >= 0 && cellIndex < static_cast<int>(m_spatialGrid.size())) {
            m_spatialGrid[cellIndex].push_back(tileIndex);
        }
    }
}

void CollisionMap::placeTile(const std::string& tileName, int worldX, int worldY, 
                              int width, int height) {
    auto it = m_tileNameToIndex.find(tileName);
    if (it == m_tileNameToIndex.end()) {
        LOG_WARN("CollisionMap::placeTile: Unknown tile name '{}'", tileName);
        return;
    }
    placeTile(it->second, worldX, worldY, width, height);
}

void CollisionMap::clearPlacedTiles() {
    m_placedTiles.clear();
    for (auto& cell : m_spatialGrid) {
        cell.clear();
    }
}

CollisionType CollisionMap::getCollisionAt(float worldX, float worldY) const {
    // Check all tiles at this position
    std::vector<const PlacedTile*> tiles = getTilesInRegion(worldX, worldY, 1.0f, 1.0f);
    
    CollisionType result = CollisionType::None;
    
    for (const PlacedTile* tile : tiles) {
        // Check if point is within tile bounds
        if (worldX >= tile->worldX && worldX < tile->worldX + tile->width &&
            worldY >= tile->worldY && worldY < tile->worldY + tile->height) {
            
            const TileDefinition& def = m_tileDefinitions[tile->tileDefIndex];
            
            // Check pixel-perfect collision if available
            if (def.pixelMask && def.pixelMask->isValid()) {
                int localX = static_cast<int>(worldX - tile->worldX);
                int localY = static_cast<int>(worldY - tile->worldY);
                if (def.pixelMask->isPixelSolid(localX, localY)) {
                    // Return the most restrictive collision type
                    if (def.collision > result) {
                        result = def.collision;
                    }
                }
            } else {
                // No pixel mask, use tile collision type directly
                if (def.collision > result) {
                    result = def.collision;
                }
            }
        }
    }
    
    return result;
}

bool CollisionMap::isBlocked(float worldX, float worldY) const {
    CollisionType col = getCollisionAt(worldX, worldY);
    return col == CollisionType::Solid || col == CollisionType::Liquid;
}

bool CollisionMap::isBlockedAABB(float x, float y, float width, float height) const {
    // Check corners and center points
    const float checkPoints[][2] = {
        {x, y},                          // Top-left
        {x + width - 1, y},              // Top-right
        {x, y + height - 1},             // Bottom-left
        {x + width - 1, y + height - 1}, // Bottom-right
        {x + width * 0.5f, y + height * 0.5f}  // Center
    };
    
    for (const auto& point : checkPoints) {
        if (isBlocked(point[0], point[1])) {
            return true;
        }
    }
    
    // For pixel-perfect, we should check more points or use mask collision
    // This is a simplified check
    return false;
}

bool CollisionMap::checkPixelCollision(float entityX, float entityY,
                                        const CollisionMask& entityMask) const {
    // Get all tiles that could potentially collide
    std::vector<const PlacedTile*> tiles = getTilesInRegion(
        entityX, entityY, 
        static_cast<float>(entityMask.getWidth()), 
        static_cast<float>(entityMask.getHeight()));
    
    for (const PlacedTile* tile : tiles) {
        const TileDefinition& def = m_tileDefinitions[tile->tileDefIndex];
        
        // Skip tiles with no collision
        if (def.collision == CollisionType::None) {
            continue;
        }
        
        if (def.pixelMask && def.pixelMask->isValid()) {
            // Pixel-perfect collision
            int offsetX = tile->worldX - static_cast<int>(entityX);
            int offsetY = tile->worldY - static_cast<int>(entityY);
            
            if (entityMask.collidesWith(*def.pixelMask, offsetX, offsetY)) {
                return true;
            }
        } else {
            // AABB vs mask collision
            int relX = static_cast<int>(entityX) - tile->worldX;
            int relY = static_cast<int>(entityY) - tile->worldY;
            
            if (entityMask.collidesWithAABB(-relX, -relY, tile->width, tile->height)) {
                return true;
            }
        }
    }
    
    return false;
}

CollisionType CollisionMap::checkMovement(float x, float y, float width, float height) const {
    std::vector<const PlacedTile*> tiles = getTilesInRegion(x, y, width, height);
    
    CollisionType result = CollisionType::None;
    
    for (const PlacedTile* tile : tiles) {
        // Check AABB overlap
        bool overlaps = !(x + width <= tile->worldX || 
                          x >= tile->worldX + tile->width ||
                          y + height <= tile->worldY || 
                          y >= tile->worldY + tile->height);
        
        if (overlaps) {
            const TileDefinition& def = m_tileDefinitions[tile->tileDefIndex];
            
            // For pixel-perfect, check the overlapping region
            if (def.pixelMask && def.pixelMask->isValid()) {
                // Calculate overlap region in tile-local coordinates
                int overlapX = std::max(0, static_cast<int>(x) - tile->worldX);
                int overlapY = std::max(0, static_cast<int>(y) - tile->worldY);
                int overlapW = std::min(tile->width, static_cast<int>(x + width) - tile->worldX) - overlapX;
                int overlapH = std::min(tile->height, static_cast<int>(y + height) - tile->worldY) - overlapY;
                
                if (def.pixelMask->collidesWithAABB(overlapX, overlapY, overlapW, overlapH)) {
                    if (def.collision > result) {
                        result = def.collision;
                    }
                }
            } else {
                // No pixel mask - use tile collision type directly
                if (def.collision > result) {
                    result = def.collision;
                }
            }
        }
    }
    
    return result;
}

TileInteraction* CollisionMap::getInteractionAt(float worldX, float worldY) {
    std::vector<const PlacedTile*> tiles = getTilesInRegion(worldX, worldY, 1.0f, 1.0f);
    
    for (const PlacedTile* tile : tiles) {
        if (worldX >= tile->worldX && worldX < tile->worldX + tile->width &&
            worldY >= tile->worldY && worldY < tile->worldY + tile->height) {
            
            // Need non-const access, so find in m_placedTiles
            for (PlacedTile& pt : m_placedTiles) {
                if (&pt == tile && pt.interaction && 
                    pt.interaction->type != InteractionType::None) {
                    return pt.interaction;
                }
            }
        }
    }
    
    return nullptr;
}

std::vector<TileInteraction*> CollisionMap::getInteractionsInRange(float x, float y, float range) {
    std::vector<TileInteraction*> result;
    
    std::vector<const PlacedTile*> tiles = getTilesInRegion(
        x - range, y - range, range * 2, range * 2);
    
    for (const PlacedTile* constTile : tiles) {
        // Find the non-const version
        for (PlacedTile& tile : m_placedTiles) {
            if (&tile == constTile && tile.interaction && 
                tile.interaction->type != InteractionType::None &&
                !tile.interaction->consumed) {
                
                // Check if tile center is within range
                float tileX = tile.worldX + tile.width * 0.5f;
                float tileY = tile.worldY + tile.height * 0.5f;
                float dx = tileX - x;
                float dy = tileY - y;
                float distSq = dx * dx + dy * dy;
                
                if (distSq <= range * range) {
                    result.push_back(tile.interaction);
                }
            }
        }
    }
    
    return result;
}

std::vector<const PlacedTile*> CollisionMap::getTilesInRegion(float x, float y,
                                                               float width, float height) const {
    std::vector<const PlacedTile*> result;
    
    // Get all grid cells that overlap with the region
    std::vector<int> cells = getGridCellsForRegion(x, y, width, height);
    
    // Collect unique tile indices
    std::vector<bool> seen(m_placedTiles.size(), false);
    
    for (int cellIndex : cells) {
        if (cellIndex >= 0 && cellIndex < static_cast<int>(m_spatialGrid.size())) {
            for (int tileIndex : m_spatialGrid[cellIndex]) {
                if (!seen[tileIndex]) {
                    seen[tileIndex] = true;
                    result.push_back(&m_placedTiles[tileIndex]);
                }
            }
        }
    }
    
    return result;
}

void CollisionMap::rebuildSpatialGrid() {
    // Clear existing grid
    for (auto& cell : m_spatialGrid) {
        cell.clear();
    }
    
    // Re-add all tiles
    for (int i = 0; i < static_cast<int>(m_placedTiles.size()); ++i) {
        const PlacedTile& tile = m_placedTiles[i];
        
        std::vector<int> cells = getGridCellsForRegion(
            static_cast<float>(tile.worldX), static_cast<float>(tile.worldY),
            static_cast<float>(tile.width), static_cast<float>(tile.height));
        
        for (int cellIndex : cells) {
            if (cellIndex >= 0 && cellIndex < static_cast<int>(m_spatialGrid.size())) {
                m_spatialGrid[cellIndex].push_back(i);
            }
        }
    }
    
    LOG_DEBUG("Rebuilt spatial grid with {} tiles in {}x{} grid", 
              m_placedTiles.size(), m_gridWidth, m_gridHeight);
}

void CollisionMap::getGridCell(float worldX, float worldY, int& cellX, int& cellY) const {
    cellX = static_cast<int>(worldX) / m_gridCellSize;
    cellY = static_cast<int>(worldY) / m_gridCellSize;
}

std::vector<int> CollisionMap::getGridCellsForRegion(float x, float y, float w, float h) const {
    std::vector<int> result;
    
    // Convert world coordinates to grid-relative coordinates
    // Grid is centered at (worldWidth/2, worldHeight/2) to handle negative coords
    float gridOffsetX = static_cast<float>(m_worldWidth) * 0.5f;
    float gridOffsetY = static_cast<float>(m_worldHeight) * 0.5f;
    
    float relX = x + gridOffsetX;
    float relY = y + gridOffsetY;
    
    // Handle negative coordinates properly - use floor division
    int startCellX = static_cast<int>(std::floor(relX / m_gridCellSize));
    int startCellY = static_cast<int>(std::floor(relY / m_gridCellSize));
    int endCellX = static_cast<int>(std::floor((relX + w) / m_gridCellSize));
    int endCellY = static_cast<int>(std::floor((relY + h) / m_gridCellSize));
    
    // Clamp to valid grid bounds
    startCellX = std::max(0, std::min(startCellX, m_gridWidth - 1));
    startCellY = std::max(0, std::min(startCellY, m_gridHeight - 1));
    endCellX = std::max(0, std::min(endCellX, m_gridWidth - 1));
    endCellY = std::max(0, std::min(endCellY, m_gridHeight - 1));
    
    for (int cy = startCellY; cy <= endCellY; ++cy) {
        for (int cx = startCellX; cx <= endCellX; ++cx) {
            result.push_back(cy * m_gridWidth + cx);
        }
    }
    
    return result;
}

} // namespace Runa
