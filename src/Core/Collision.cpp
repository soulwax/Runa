

#include "../runapch.h"
#include "Collision.h"
#include <cmath>
#include <algorithm>

namespace Runa {


bool Collision::checkAABB(const AABB& a, const AABB& b) {
    return a.intersects(b);
}

void Collision::resolveAABB(AABB& a, const AABB& b, float& outDx, float& outDy) {

    float overlapX = std::min(a.x + a.width, b.x + b.width) - std::max(a.x, b.x);
    float overlapY = std::min(a.y + a.height, b.y + b.height) - std::max(a.y, b.y);


    if (overlapX < overlapY) {

        if (a.x < b.x) {
            outDx = -overlapX;
        } else {
            outDx = overlapX;
        }
        outDy = 0.0f;
    } else {

        outDx = 0.0f;
        if (a.y < b.y) {
            outDy = -overlapY;
        } else {
            outDy = overlapY;
        }
    }
}

bool Collision::checkTileCollision(const AABB& box, const TileMap& tilemap, int tileSize) {

    int startX = static_cast<int>(box.x / tileSize);
    int endX = static_cast<int>((box.x + box.width) / tileSize);
    int startY = static_cast<int>(box.y / tileSize);
    int endY = static_cast<int>((box.y + box.height) / tileSize);


    for (int ty = startY; ty <= endY; ++ty) {
        for (int tx = startX; tx <= endX; ++tx) {

            if (tx < 0 || tx >= tilemap.getWidth() || ty < 0 || ty >= tilemap.getHeight()) {
                continue;
            }

            int tileIndex = tilemap.getTile(tx, ty);
            if (tilemap.isSolidTile(tileIndex)) {

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


    int startX = static_cast<int>(entityBox.x / tileSize);
    int endX = static_cast<int>((entityBox.x + entityBox.width) / tileSize);
    int startY = static_cast<int>(entityBox.y / tileSize);
    int endY = static_cast<int>((entityBox.y + entityBox.height) / tileSize);


    for (int ty = startY; ty <= endY; ++ty) {
        for (int tx = startX; tx <= endX; ++tx) {

            if (tx < 0 || tx >= tilemap.getWidth() || ty < 0 || ty >= tilemap.getHeight()) {
                continue;
            }

            int tileIndex = tilemap.getTile(tx, ty);
            if (tilemap.isSolidTile(tileIndex)) {

                AABB tileBox{
                    static_cast<float>(tx * tileSize),
                    static_cast<float>(ty * tileSize),
                    static_cast<float>(tileSize),
                    static_cast<float>(tileSize)
                };

                if (entityBox.intersects(tileBox)) {
                    float dx, dy;
                    resolveAABB(entityBox, tileBox, dx, dy);


                    entity.move(dx, dy);


                    entityBox = entity.getAABB();
                }
            }
        }
    }
}


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

}
