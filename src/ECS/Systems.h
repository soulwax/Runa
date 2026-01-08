// File: src/ECS/Systems.h
// System function declarations for EnTT Entity Component System

#ifndef RUNA_ECS_SYSTEMS_H
#define RUNA_ECS_SYSTEMS_H

#include "../RunaAPI.h"
#include <entt/entt.hpp>

namespace Runa {
    class Input;
    class SpriteBatch;
    class Camera;
    class TileMap;
    class Texture;
}

namespace Runa::ECS::Systems {

// ============================================================================
// Input System
// ============================================================================

// Update player input and set velocities
RUNA_API void updatePlayerInput(entt::registry& registry, Input& input, float dt);

// ============================================================================
// Physics System
// ============================================================================

// Apply velocity to position
RUNA_API void updateMovement(entt::registry& registry, float dt);

// ============================================================================
// Animation System
// ============================================================================

// Update sprite animation frames
RUNA_API void updateAnimation(entt::registry& registry, float dt);

// ============================================================================
// Collision System
// ============================================================================

// Check and resolve tile collisions
RUNA_API void updateTileCollisions(entt::registry& registry, const TileMap& tilemap, int tileSize);

// Check entity-to-entity collisions
RUNA_API void updateEntityCollisions(entt::registry& registry);

// ============================================================================
// Rendering System
// ============================================================================

// Render all sprites to sprite batch (with camera transform)
// whitePixelTexture: Used for rendering entities without sprite sheets as colored rectangles
RUNA_API void renderSprites(entt::registry& registry, SpriteBatch& batch, Camera& camera, Texture* whitePixelTexture = nullptr);

// ============================================================================
// Camera System
// ============================================================================

// Update camera to follow target entity
RUNA_API void updateCameraFollow(entt::registry& registry, Camera& camera, float dt);

} // namespace Runa::ECS::Systems

#endif // RUNA_ECS_SYSTEMS_H
