// File: src/ECS/Components.h

#ifndef RUNA_ECS_COMPONENTS_H
#define RUNA_ECS_COMPONENTS_H

#include "../RunaAPI.h"
#include <string>
#include <entt/entt.hpp>

namespace Runa {
    class SpriteSheet;
}

namespace Runa::ECS {






struct RUNA_API Position {
    float x = 0.0f;
    float y = 0.0f;
};


struct RUNA_API Velocity {
    float x = 0.0f;
    float y = 0.0f;
};


struct RUNA_API Size {
    float width = 16.0f;
    float height = 16.0f;
};


struct RUNA_API Transform {
    float x = 0.0f;
    float y = 0.0f;
    float rotation = 0.0f;
    float scaleX = 1.0f;
    float scaleY = 1.0f;
};






struct RUNA_API Sprite {
    const SpriteSheet* spriteSheet = nullptr;
    std::string spriteName;
    float tintR = 1.0f;
    float tintG = 1.0f;
    float tintB = 1.0f;
    float tintA = 1.0f;
    bool flipX = false;  // Flip sprite horizontally (mirror on X axis)
    bool flipY = false;  // Flip sprite vertically (mirror on Y axis)
};


struct RUNA_API Animation {
    int currentFrame = 0;
    float animationTime = 0.0f;
    float frameRate = 10.0f;
    bool loop = true;
};






struct RUNA_API AABB {
    float offsetX = 0.0f;
    float offsetY = 0.0f;
    float width = 16.0f;
    float height = 16.0f;


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


struct RUNA_API CollisionLayer {
    uint32_t layer = 0x00000001;
    uint32_t mask = 0xFFFFFFFF;
};

/**
 * Collider component for entity collision
 */
struct RUNA_API Collider {
    enum class Type : uint8_t {
        None = 0,
        Solid,          // Blocks movement
        Trigger,        // Detects overlap but doesn't block
        Kinematic       // Can push/be pushed
    };
    
    Type type = Type::Solid;
    bool enabled = true;
    bool isTrigger = false;
    
    // Collision response flags
    bool blocksMovement = true;
    bool detectsOverlap = true;
};

/**
 * Interactable component for entities that can be interacted with
 */
struct RUNA_API Interactable {
    enum class Type : uint8_t {
        None = 0,
        Read,           // Sign, book - displays text
        Container,      // Chest, barrel - contains items
        Teleport,       // Door, portal - moves player
        Toggle,         // Switch, lever - toggles state
        Pickup,         // Item on ground - adds to inventory
        Talk            // NPC - starts dialogue
    };
    
    Type type = Type::None;
    std::string data;           // Type-specific data (message, items, etc.)
    std::string targetScene;    // For teleport
    float targetX = 0.0f;       // For teleport
    float targetY = 0.0f;       // For teleport
    float interactionRange = 24.0f;  // How close player must be
    bool oneTime = false;       // If true, consumed after use
    bool consumed = false;      // Runtime flag
    bool requiresFacing = true; // Must face the object to interact
};

/**
 * Component to mark an entity as able to interact with Interactables
 */
struct RUNA_API CanInteract {
    float range = 32.0f;        // How far can reach
    bool isInteracting = false; // Currently in interaction
};

/**
 * Collision event data (passed to handlers)
 */
struct RUNA_API CollisionEvent {
    entt::entity other = entt::null;
    float overlapX = 0.0f;
    float overlapY = 0.0f;
    bool fromLeft = false;
    bool fromRight = false;
    bool fromTop = false;
    bool fromBottom = false;
};



struct RUNA_API PlayerInput {
    float speed = 100.0f;
};


struct RUNA_API CameraTarget {
    float smoothing = 0.15f;
};






struct RUNA_API Active {};


struct RUNA_API Projectile {};
struct RUNA_API Pickup {};
struct RUNA_API Static {};

}

#endif
