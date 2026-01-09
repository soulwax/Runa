

#ifndef RUNA_CORE_COLLISION_H
#define RUNA_CORE_COLLISION_H

#include "../RunaAPI.h"
#include "Entity.h"
#include "Graphics/TileMap.h"
#include <vector>

namespace Runa {


class RUNA_API Collision {
public:

    static bool checkAABB(const AABB& a, const AABB& b);



    static void resolveAABB(AABB& a, const AABB& b, float& outDx, float& outDy);


    static bool checkTileCollision(const AABB& box, const TileMap& tilemap, int tileSize);


    static void resolveTileCollision(Entity& entity, const TileMap& tilemap, int tileSize);
};


class RUNA_API CollisionManager {
public:
    CollisionManager() = default;
    ~CollisionManager() = default;


    void checkTileCollisions(Entity& entity, const TileMap& tilemap, int tileSize);


    bool checkEntityCollision(const Entity& a, const Entity& b);


    std::vector<Entity*> checkEntityCollisions(Entity& entity, const std::vector<Entity*>& others);
};

}

#endif
