


#ifndef RUNA_ECS_COMPONENTS_H
#define RUNA_ECS_COMPONENTS_H

#include "../RunaAPI.h"
#include <string>

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
