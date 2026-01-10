#ifndef RUNA_COLLISION_COLLISIONMAP_H
#define RUNA_COLLISION_COLLISIONMAP_H

#include "../RunaAPI.h"
#include "CollisionMask.h"
#include "../runapch.h"
#include <vector>
#include <unordered_map>
#include <string>
#include <memory>

namespace Runa {

/**
 * Collision types for tiles
 */
enum class CollisionType : uint8_t {
    None = 0,       // Passable
    Solid,          // Blocks all movement
    Liquid,         // Water/lava - may slow or block
    Platform,       // One-way platform (pass through from below)
    Trigger,        // Triggers an event but doesn't block
    Hazard          // Damages on contact
};

/**
 * Interaction types for tiles
 */
enum class InteractionType : uint8_t {
    None = 0,
    Read,           // Sign, book - displays text
    Container,      // Chest, barrel - contains items
    Teleport,       // Door, portal - moves player
    Toggle,         // Switch, lever - toggles state
    Pickup,         // Item on ground - adds to inventory
    Talk            // NPC - starts dialogue
};

/**
 * Interaction data for a tile
 */
struct TileInteraction {
    InteractionType type = InteractionType::None;
    std::string data;           // Type-specific data (message, loot table, target, etc.)
    std::string targetScene;    // For teleport
    float targetX = 0.0f;       // For teleport
    float targetY = 0.0f;       // For teleport
    bool oneTime = false;       // If true, interaction is consumed after use
    bool consumed = false;      // Runtime flag - has this been used?
};

/**
 * Tile collision/interaction definition (from YAML)
 */
struct TileDefinition {
    std::string name;
    CollisionType collision = CollisionType::None;
    TileInteraction interaction;
    std::shared_ptr<CollisionMask> pixelMask;  // Optional pixel-perfect mask
};

/**
 * A placed tile in the collision map
 */
struct PlacedTile {
    int tileDefIndex = -1;      // Index into tile definitions
    int worldX = 0;             // World position (pixels)
    int worldY = 0;
    int width = 0;              // Tile size (pixels)
    int height = 0;
    TileInteraction* interaction = nullptr;  // Runtime interaction state (can be modified)
};

/**
 * CollisionMap manages collision data for an entire scene.
 * Supports both tile-based and pixel-perfect collision.
 */
class RUNA_API CollisionMap {
public:
    CollisionMap() = default;
    CollisionMap(int worldWidth, int worldHeight, int tileSize);
    
    // Add a tile definition (from YAML)
    int addTileDefinition(const TileDefinition& def);
    TileDefinition* getTileDefinition(int index);
    TileDefinition* getTileDefinition(const std::string& name);
    
    // Place a tile in the world
    void placeTile(int tileDefIndex, int worldX, int worldY, int width, int height);
    void placeTile(const std::string& tileName, int worldX, int worldY, int width, int height);
    
    // Remove all placed tiles
    void clearPlacedTiles();
    
    // Collision queries
    CollisionType getCollisionAt(float worldX, float worldY) const;
    bool isBlocked(float worldX, float worldY) const;
    bool isBlockedAABB(float x, float y, float width, float height) const;
    
    // Pixel-perfect collision check
    bool checkPixelCollision(float entityX, float entityY, 
                             const CollisionMask& entityMask) const;
    
    // Get collision type for movement (checks AABB and returns most restrictive type)
    CollisionType checkMovement(float x, float y, float width, float height) const;
    
    // Interaction queries
    TileInteraction* getInteractionAt(float worldX, float worldY);
    std::vector<TileInteraction*> getInteractionsInRange(float x, float y, float range);
    
    // Get tiles in a region (for rendering or broad-phase collision)
    std::vector<const PlacedTile*> getTilesInRegion(float x, float y, 
                                                     float width, float height) const;
    
    // Spatial grid for fast lookups
    void rebuildSpatialGrid();
    
    int getTileSize() const { return m_tileSize; }
    int getWorldWidth() const { return m_worldWidth; }
    int getWorldHeight() const { return m_worldHeight; }

private:
    int m_worldWidth = 0;
    int m_worldHeight = 0;
    int m_tileSize = 16;
    
    // Tile definitions (templates)
    std::vector<TileDefinition> m_tileDefinitions;
    std::unordered_map<std::string, int> m_tileNameToIndex;
    
    // Placed tiles in the world
    std::vector<PlacedTile> m_placedTiles;
    
    // Spatial grid for fast collision queries
    // Grid cell = list of indices into m_placedTiles
    int m_gridCellSize = 64;  // Pixels per grid cell
    int m_gridWidth = 0;
    int m_gridHeight = 0;
    std::vector<std::vector<int>> m_spatialGrid;
    
    // Get grid cell indices for a world position
    void getGridCell(float worldX, float worldY, int& cellX, int& cellY) const;
    std::vector<int> getGridCellsForRegion(float x, float y, float w, float h) const;
};

} // namespace Runa

#endif // RUNA_COLLISION_COLLISIONMAP_H
