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
    // RADICAL APPROACH: Check ALL tiles directly (brute force)
    // This eliminates any spatial grid bugs
    CollisionType result = CollisionType::None;
    
    // Calculate entity AABB bounds
    float entityLeft = x;
    float entityRight = x + width;
    float entityTop = y;
    float entityBottom = y + height;
    
    static int checkCount = 0;
    bool shouldLog = (checkCount++ < 20);
    
    if (shouldLog) {
        LOG_INFO("checkMovement: Entity AABB({}, {} {}x{}) checking {} tiles", 
                 x, y, width, height, m_placedTiles.size());
    }
    
    // Check every single tile
    for (const PlacedTile& tile : m_placedTiles) {
        // Simple AABB overlap check
        float tileLeft = static_cast<float>(tile.worldX);
        float tileRight = static_cast<float>(tile.worldX + tile.width);
        float tileTop = static_cast<float>(tile.worldY);
        float tileBottom = static_cast<float>(tile.worldY + tile.height);
        
        // Check if AABBs overlap
        bool overlaps = !(entityRight <= tileLeft || 
                          entityLeft >= tileRight ||
                          entityBottom <= tileTop || 
                          entityTop >= tileBottom);
        
        if (overlaps) {
            const TileDefinition& def = m_tileDefinitions[tile.tileDefIndex];
            
            if (shouldLog) {
                LOG_INFO("  AABB overlap with tile at ({}, {} {}x{})", 
                         tile.worldX, tile.worldY, tile.width, tile.height);
            }
            
            // For pixel-perfect, check the overlapping region
            if (def.pixelMask && def.pixelMask->isValid()) {
                // Calculate overlap region in tile-local coordinates
                float overlapLeft = std::max(entityLeft, tileLeft) - tileLeft;
                float overlapTop = std::max(entityTop, tileTop) - tileTop;
                float overlapRight = std::min(entityRight, tileRight) - tileLeft;
                float overlapBottom = std::min(entityBottom, tileBottom) - tileTop;
                
                int overlapX = static_cast<int>(overlapLeft);
                int overlapY = static_cast<int>(overlapTop);
                int overlapW = static_cast<int>(overlapRight) - overlapX;
                int overlapH = static_cast<int>(overlapBottom) - overlapY;
                
                if (shouldLog) {
                    LOG_INFO("    Pixel check: overlap region ({}, {} {}x{}) in tile-local coords", 
                             overlapX, overlapY, overlapW, overlapH);
                }
                
                // Only check if overlap region is valid
                if (overlapW > 0 && overlapH > 0 && 
                    overlapX >= 0 && overlapY >= 0 &&
                    overlapX + overlapW <= tile.width &&
                    overlapY + overlapH <= tile.height) {
                    
                    bool pixelCollision = def.pixelMask->collidesWithAABB(overlapX, overlapY, overlapW, overlapH);
                    
                    if (shouldLog) {
                        LOG_INFO("    Pixel collision result: {}", pixelCollision ? "COLLISION" : "NO COLLISION");
                    }
                    
                    if (pixelCollision) {
                        if (def.collision > result) {
                            result = def.collision;
                        }
                        if (shouldLog) {
                            LOG_INFO("  *** COLLISION DETECTED! Type: {}", 
                                     result == CollisionType::Solid ? "SOLID" : "OTHER");
                        }
                    }
                } else {
                    if (shouldLog) {
                        LOG_WARN("    Invalid overlap region: ({}, {} {}x{}) for tile {}x{}", 
                                 overlapX, overlapY, overlapW, overlapH, tile.width, tile.height);
                    }
                }
            } else {
                // No pixel mask - use tile collision type directly
                if (shouldLog) {
                    LOG_INFO("    No pixel mask, using solid collision");
                }
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
    
    static int debugQueryCount = 0;
    if (debugQueryCount++ < 10) {
        LOG_DEBUG("getTilesInRegion: Query region ({}, {} {}x{}) -> {} grid cells", 
                 x, y, width, height, cells.size());
    }
    
    // Collect unique tile indices
    std::vector<bool> seen(m_placedTiles.size(), false);
    
    for (int cellIndex : cells) {
        if (cellIndex >= 0 && cellIndex < static_cast<int>(m_spatialGrid.size())) {
            for (int tileIndex : m_spatialGrid[cellIndex]) {
                if (!seen[tileIndex]) {
                    seen[tileIndex] = true;
                    const PlacedTile& tile = m_placedTiles[tileIndex];
                    if (debugQueryCount <= 10) {
                        LOG_DEBUG("  Found tile {} at ({}, {} {}x{})", 
                                 tileIndex, tile.worldX, tile.worldY, tile.width, tile.height);
                    }
                    result.push_back(&tile);
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
