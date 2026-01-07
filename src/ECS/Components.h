// File: src/ECS/Components.h
// Component definitions for EnTT Entity Component System

#ifndef RUNA_ECS_COMPONENTS_H
#define RUNA_ECS_COMPONENTS_H

#include "../RunaAPI.h"
#include <string>

namespace Runa {
    class SpriteSheet;
}

namespace Runa::ECS {

// ============================================================================
// Transform & Physics Components
// ============================================================================

// Position in world space (center of entity)
struct RUNA_API Position {
    float x = 0.0f;
    float y = 0.0f;
};

// Velocity for physics-based movement
struct RUNA_API Velocity {
    float x = 0.0f;
    float y = 0.0f;
};

// Size for collision and rendering (width and height)
struct RUNA_API Size {
    float width = 16.0f;
    float height = 16.0f;
};

// Combined transform (optional - for entities that need all three)
struct RUNA_API Transform {
    float x = 0.0f;
    float y = 0.0f;
    float rotation = 0.0f;  // For future use
    float scaleX = 1.0f;
    float scaleY = 1.0f;
};

// ============================================================================
// Rendering Components
// ============================================================================

// Sprite rendering data
struct RUNA_API Sprite {
    const SpriteSheet* spriteSheet = nullptr;
    std::string spriteName;
    float tintR = 1.0f;
    float tintG = 1.0f;
    float tintB = 1.0f;
    float tintA = 1.0f;
};

// Animation state
struct RUNA_API Animation {
    int currentFrame = 0;
    float animationTime = 0.0f;
    float frameRate = 10.0f;  // FPS for animation
    bool loop = true;
};

// ============================================================================
// Collision Components
// ============================================================================

// AABB collision box (axis-aligned bounding box)
struct RUNA_API AABB {
    float offsetX = 0.0f;  // Offset from position
    float offsetY = 0.0f;
    float width = 16.0f;
    float height = 16.0f;

    // Helper to get world-space AABB
    struct WorldAABB {
        float x, y, width, height;

        bool intersects(const WorldAABB& other) const {
            return (x < other.x + other.width &&
                    x + width > other.x &&
                    y < other.y + other.height &&
                    y + height > other.y);
        }
    };
};

// Collision layer/mask for filtering
struct RUNA_API CollisionLayer {
    uint32_t layer = 0x00000001;  // Which layer this entity is on
    uint32_t mask = 0xFFFFFFFF;   // Which layers this entity collides with
};

// ============================================================================
// Input Components
// ============================================================================

// Player input controller data
struct RUNA_API PlayerInput {
    float speed = 100.0f;  // Movement speed in pixels/sec
};

// Camera target (marks entity as camera follow target)
struct RUNA_API CameraTarget {
    float smoothing = 0.15f;  // Lerp smoothing factor
};

// ============================================================================
// State Components (Tags)
// ============================================================================

// Active state - inactive entities are skipped by systems
struct RUNA_API Active {};

// Tag components (zero-sized)
struct RUNA_API Projectile {};
struct RUNA_API Pickup {};
struct RUNA_API Static {};  // For non-moving objects

} // namespace Runa::ECS

#endif // RUNA_ECS_COMPONENTS_H
