// File: src/ECS/Registry.h
// Registry wrapper with helper functions for EnTT Entity Component System

#ifndef RUNA_ECS_REGISTRY_H
#define RUNA_ECS_REGISTRY_H

#include "../RunaAPI.h"
#include "Components.h"
#include <entt/entt.hpp>
#include <string>

namespace Runa {
    class SpriteSheet;
}

namespace Runa::ECS {

// Convenient wrapper around entt::registry with helper functions
class RUNA_API EntityRegistry {
public:
    EntityRegistry() = default;
    ~EntityRegistry() = default;

    // Get underlying registry
    entt::registry& getRegistry() { return m_registry; }
    const entt::registry& getRegistry() const { return m_registry; }

    // ========================================================================
    // Entity Creation Helpers
    // ========================================================================

    // Create a basic entity with position
    entt::entity createEntity(float x = 0.0f, float y = 0.0f);

    // Create a sprite entity
    entt::entity createSpriteEntity(float x, float y,
                                   const SpriteSheet* spriteSheet,
                                   const std::string& spriteName,
                                   float width = 16.0f, float height = 16.0f);

    // Create a player entity
    entt::entity createPlayer(float x, float y,
                              const SpriteSheet* spriteSheet,
                              const std::string& spriteName,
                              float speed = 120.0f);

    // Destroy entity
    void destroyEntity(entt::entity entity);

    // ========================================================================
    // Component Helpers
    // ========================================================================

    // Add sprite to entity
    void addSprite(entt::entity entity, const SpriteSheet* spriteSheet,
                   const std::string& spriteName);

    // Add animation to entity
    void addAnimation(entt::entity entity, float frameRate = 10.0f, bool loop = true);

    // Add collision box to entity
    void addCollision(entt::entity entity, float width, float height,
                      float offsetX = 0.0f, float offsetY = 0.0f);

    // ========================================================================
    // Utility
    // ========================================================================

    // Get entity count
    size_t getEntityCount() const { return 0; /* TODO: Fix for EnTT version */ }

    // Clear all entities
    void clear() { m_registry.clear(); }

private:
    entt::registry m_registry;
};

} // namespace Runa::ECS

#endif // RUNA_ECS_REGISTRY_H
