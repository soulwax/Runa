// File: src/Core/Collision.h

#ifndef RUNA_CORE_COLLISION_H
#define RUNA_CORE_COLLISION_H

#include "RunaAPI.h"
#include "Entity.h"
#include "Graphics/TileMap.h"
#include <vector>

namespace Runa {

// Static collision utilities
class RUNA_API Collision {
public:
    // AABB vs AABB
    static bool checkAABB(const AABB& a, const AABB& b);

    // Resolve collision by pushing 'a' out of 'b'
    // Returns penetration depth (dx, dy)
    static void resolveAABB(AABB& a, const AABB& b, float& outDx, float& outDy);

    // Check if AABB overlaps with solid tiles in tilemap
    static bool checkTileCollision(const AABB& box, const TileMap& tilemap, int tileSize);

    // Resolve entity-tilemap collision (sweep collision)
    static void resolveTileCollision(Entity& entity, const TileMap& tilemap, int tileSize);
};

// Simple collision manager for entities
class RUNA_API CollisionManager {
public:
    CollisionManager() = default;
    ~CollisionManager() = default;

    // Check entity against tilemap and resolve
    void checkTileCollisions(Entity& entity, const TileMap& tilemap, int tileSize);

    // Check two entities
    bool checkEntityCollision(const Entity& a, const Entity& b);

    // Check entity against list of entities
    std::vector<Entity*> checkEntityCollisions(Entity& entity, const std::vector<Entity*>& others);
};

} // namespace Runa

#endif // RUNA_CORE_COLLISION_H
