// File: src/ECS/Systems.h

#ifndef RUNA_ECS_SYSTEMS_H
#define RUNA_ECS_SYSTEMS_H

#include "../RunaAPI.h"
#include <entt/entt.hpp>

namespace Runa {
    class Input;
    class Keybindings;
    class SpriteBatch;
    class Camera;
    class TileMap;
    class Texture;
}

namespace Runa::ECS::Systems {






RUNA_API void updatePlayerInput(entt::registry& registry, Input& input, float dt);
RUNA_API void updatePlayerInput(entt::registry& registry, Input& input, float dt, Keybindings* keybindings);






RUNA_API void updateMovement(entt::registry& registry, float dt);






RUNA_API void updateAnimation(entt::registry& registry, float dt);






RUNA_API void updateTileCollisions(entt::registry& registry, const TileMap& tilemap, int tileSize);


RUNA_API void updateEntityCollisions(entt::registry& registry);







RUNA_API void renderSprites(entt::registry& registry, SpriteBatch& batch, Camera& camera, Texture* whitePixelTexture = nullptr);






RUNA_API void updateCameraFollow(entt::registry& registry, Camera& camera, float dt);

}

#endif
