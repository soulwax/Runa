// File: src/Core/Collision.cpp

#include "runapch.h"
#include "Collision.h"
#include <cmath>
#include <algorithm>

namespace Runa {

// Collision static methods
bool Collision::checkAABB(const AABB& a, const AABB& b) {
    return a.intersects(b);
}

void Collision::resolveAABB(AABB& a, const AABB& b, float& outDx, float& outDy) {
    // Calculate overlap on each axis
    float overlapX = std::min(a.x + a.width, b.x + b.width) - std::max(a.x, b.x);
    float overlapY = std::min(a.y + a.height, b.y + b.height) - std::max(a.y, b.y);

    // Push out on the axis with smallest penetration
    if (overlapX < overlapY) {
        // Push horizontally
        if (a.x < b.x) {
            outDx = -overlapX;  // Push left
        } else {
            outDx = overlapX;   // Push right
        }
        outDy = 0.0f;
    } else {
        // Push vertically
        outDx = 0.0f;
        if (a.y < b.y) {
            outDy = -overlapY;  // Push up
        } else {
            outDy = overlapY;   // Push down
        }
    }
}

bool Collision::checkTileCollision(const AABB& box, const TileMap& tilemap, int tileSize) {
    // Get tile coordinates that entity overlaps
    int startX = static_cast<int>(box.x / tileSize);
    int endX = static_cast<int>((box.x + box.width) / tileSize);
    int startY = static_cast<int>(box.y / tileSize);
    int endY = static_cast<int>((box.y + box.height) / tileSize);

    // Check each tile in range
    for (int ty = startY; ty <= endY; ++ty) {
        for (int tx = startX; tx <= endX; ++tx) {
            // Check if tile is in bounds
            if (tx < 0 || tx >= tilemap.getWidth() || ty < 0 || ty >= tilemap.getHeight()) {
                continue;
            }

            int tileIndex = tilemap.getTile(tx, ty);
            if (tilemap.isSolidTile(tileIndex)) {
                // Tile is solid - check collision
                AABB tileBox{
                    static_cast<float>(tx * tileSize),
                    static_cast<float>(ty * tileSize),
                    static_cast<float>(tileSize),
                    static_cast<float>(tileSize)
                };

                if (box.intersects(tileBox)) {
                    return true;
                }
            }
        }
    }

    return false;
}

void Collision::resolveTileCollision(Entity& entity, const TileMap& tilemap, int tileSize) {
    AABB entityBox = entity.getAABB();

    // Get tile coordinates that entity overlaps
    int startX = static_cast<int>(entityBox.x / tileSize);
    int endX = static_cast<int>((entityBox.x + entityBox.width) / tileSize);
    int startY = static_cast<int>(entityBox.y / tileSize);
    int endY = static_cast<int>((entityBox.y + entityBox.height) / tileSize);

    // Check each tile and resolve collisions
    for (int ty = startY; ty <= endY; ++ty) {
        for (int tx = startX; tx <= endX; ++tx) {
            // Check if tile is in bounds
            if (tx < 0 || tx >= tilemap.getWidth() || ty < 0 || ty >= tilemap.getHeight()) {
                continue;
            }

            int tileIndex = tilemap.getTile(tx, ty);
            if (tilemap.isSolidTile(tileIndex)) {
                // Tile is solid - resolve collision
                AABB tileBox{
                    static_cast<float>(tx * tileSize),
                    static_cast<float>(ty * tileSize),
                    static_cast<float>(tileSize),
                    static_cast<float>(tileSize)
                };

                if (entityBox.intersects(tileBox)) {
                    float dx, dy;
                    resolveAABB(entityBox, tileBox, dx, dy);

                    // Apply resolution
                    entity.move(dx, dy);

                    // Update entity box for next collision check
                    entityBox = entity.getAABB();
                }
            }
        }
    }
}

// CollisionManager methods
void CollisionManager::checkTileCollisions(Entity& entity, const TileMap& tilemap, int tileSize) {
    Collision::resolveTileCollision(entity, tilemap, tileSize);
}

bool CollisionManager::checkEntityCollision(const Entity& a, const Entity& b) {
    return a.collidesWith(b);
}

std::vector<Entity*> CollisionManager::checkEntityCollisions(Entity& entity, const std::vector<Entity*>& others) {
    std::vector<Entity*> collisions;

    for (Entity* other : others) {
        if (other && other != &entity && other->isActive()) {
            if (entity.collidesWith(*other)) {
                collisions.push_back(other);
            }
        }
    }

    return collisions;
}

} // namespace Runa
