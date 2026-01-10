// File: src/ECS/Systems.h

#ifndef RUNA_ECS_SYSTEMS_H
#define RUNA_ECS_SYSTEMS_H

#include "../RunaAPI.h"
#include <entt/entt.hpp>
#include <functional>
#include <vector>

namespace Runa {
    class Input;
    class Keybindings;
    class SpriteBatch;
    class Camera;
    class TileMap;
    class Texture;
    class CollisionMap;
}

namespace Runa::ECS {
    struct CollisionEvent;
    struct Interactable;
}

// Forward declaration for tile interaction
namespace Runa {
    struct TileInteraction;
}

namespace Runa::ECS::Systems {






RUNA_API void updatePlayerInput(entt::registry& registry, Input& input, float dt);
RUNA_API void updatePlayerInput(entt::registry& registry, Input& input, float dt, Keybindings* keybindings);






RUNA_API void updateMovement(entt::registry& registry, float dt);






RUNA_API void updateAnimation(entt::registry& registry, float dt);






RUNA_API void updateTileCollisions(entt::registry& registry, const TileMap& tilemap, int tileSize);

RUNA_API void updateEntityCollisions(entt::registry& registry);

/**
 * Update collision with CollisionMap (pixel-perfect tile collision)
 * @param registry ECS registry
 * @param collisionMap The collision map with tile data
 * @param dt Delta time for movement prediction
 * @param onCollision Optional callback when collision occurs
 */
RUNA_API void updateMapCollision(entt::registry& registry, CollisionMap& collisionMap, float dt,
                                  std::function<void(entt::entity, const CollisionEvent&)> onCollision = nullptr);

/**
 * Check and resolve entity-to-entity collisions
 * @param registry ECS registry  
 * @param onCollision Optional callback for collision events
 */
RUNA_API void updateEntityToEntityCollision(entt::registry& registry,
                                             std::function<void(entt::entity, entt::entity, const CollisionEvent&)> onCollision = nullptr);

/**
 * Update interaction system - detects when player can interact and handles input
 * @param registry ECS registry
 * @param input Input handler
 * @param interactionKey Key to trigger interaction
 * @param onInteract Callback when interaction occurs
 */
RUNA_API void updateInteraction(entt::registry& registry, Input& input, int interactionKey,
                                 std::function<void(entt::entity, entt::entity, Interactable&)> onInteract);

/**
 * Update interaction with CollisionMap tiles
 * @param registry ECS registry
 * @param collisionMap The collision map with interaction data
 * @param input Input handler
 * @param interactionKey Key to trigger interaction
 * @param onInteract Callback when tile interaction occurs
 */
RUNA_API void updateTileInteraction(entt::registry& registry, CollisionMap& collisionMap,
                                     Input& input, int interactionKey,
                                     std::function<void(entt::entity, TileInteraction&)> onInteract);

/**
 * Get all interactable entities in range of an entity
 */
RUNA_API std::vector<entt::entity> getInteractablesInRange(entt::registry& registry, 
                                                            entt::entity source, float range);







RUNA_API void renderSprites(entt::registry& registry, SpriteBatch& batch, Camera& camera, Texture* whitePixelTexture = nullptr);






RUNA_API void updateCameraFollow(entt::registry& registry, Camera& camera, float dt);

}

#endif
