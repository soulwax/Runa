// File: src/ECS/Systems.cpp
// System implementations for EnTT Entity Component System

#include "../runapch.h"
#include "Systems.h"
#include "Components.h"
#include "../Core/Input.h"
#include "../Graphics/SpriteBatch.h"
#include "../Graphics/Camera.h"
#include "../Graphics/TileMap.h"
#include "../Core/Log.h"
#include <cmath>

namespace Runa::ECS::Systems {

// ============================================================================
// Input System
// ============================================================================

void updatePlayerInput(entt::registry& registry, Input& input, float dt) {
    auto view = registry.view<PlayerInput, Velocity, Active>();

    for (auto entity : view) {
        auto& playerInput = view.get<PlayerInput>(entity);
        auto& velocity = view.get<Velocity>(entity);

        // Read input for movement (WASD or Arrow keys)
        float moveX = 0.0f;
        float moveY = 0.0f;

        if (input.isKeyDown(SDLK_A) || input.isKeyDown(SDLK_LEFT)) {
            moveX -= 1.0f;
        }
        if (input.isKeyDown(SDLK_D) || input.isKeyDown(SDLK_RIGHT)) {
            moveX += 1.0f;
        }
        if (input.isKeyDown(SDLK_W) || input.isKeyDown(SDLK_UP)) {
            moveY -= 1.0f;
        }
        if (input.isKeyDown(SDLK_S) || input.isKeyDown(SDLK_DOWN)) {
            moveY += 1.0f;
        }

        // Normalize diagonal movement
        if (moveX != 0.0f && moveY != 0.0f) {
            float length = std::sqrt(moveX * moveX + moveY * moveY);
            moveX /= length;
            moveY /= length;
        }

        // Set velocity
        velocity.x = moveX * playerInput.speed;
        velocity.y = moveY * playerInput.speed;
    }
}

// ============================================================================
// Physics System
// ============================================================================

void updateMovement(entt::registry& registry, float dt) {
    auto view = registry.view<Position, Velocity, Active>();

    for (auto entity : view) {
        auto& pos = view.get<Position>(entity);
        auto& vel = view.get<Velocity>(entity);

        // Apply velocity to position
        pos.x += vel.x * dt;
        pos.y += vel.y * dt;
    }
}

// ============================================================================
// Animation System
// ============================================================================

void updateAnimation(entt::registry& registry, float dt) {
    auto view = registry.view<Sprite, Animation, Active>();

    for (auto entity : view) {
        auto& sprite = view.get<Sprite>(entity);
        auto& anim = view.get<Animation>(entity);

        // Skip if no sprite sheet
        if (!sprite.spriteSheet || sprite.spriteName.empty()) {
            continue;
        }

        const auto* spriteData = sprite.spriteSheet->getSprite(sprite.spriteName);
        if (!spriteData || spriteData->frames.empty()) {
            continue;
        }

        // Update animation time
        anim.animationTime += dt;

        // Calculate current frame
        if (spriteData->frames.size() > 1) {
            float frameDuration = 1.0f / anim.frameRate;
            int totalFrames = static_cast<int>(spriteData->frames.size());

            if (anim.loop) {
                anim.currentFrame = static_cast<int>(anim.animationTime / frameDuration) % totalFrames;
            } else {
                anim.currentFrame = std::min(static_cast<int>(anim.animationTime / frameDuration), totalFrames - 1);
            }
        } else {
            anim.currentFrame = 0;
        }
    }
}

// ============================================================================
// Collision System
// ============================================================================

void updateTileCollisions(entt::registry& registry, const TileMap& tilemap, int tileSize) {
    auto view = registry.view<Position, Size, Active>();

    for (auto entity : view) {
        auto& pos = view.get<Position>(entity);
        auto& size = view.get<Size>(entity);

        // Get AABB offset if present
        float offsetX = 0.0f;
        float offsetY = 0.0f;
        float boxWidth = size.width;
        float boxHeight = size.height;

        if (auto* aabb = registry.try_get<AABB>(entity)) {
            offsetX = aabb->offsetX;
            offsetY = aabb->offsetY;
            boxWidth = aabb->width;
            boxHeight = aabb->height;
        }

        // Calculate world-space AABB
        float worldX = pos.x + offsetX;
        float worldY = pos.y + offsetY;

        // Get tile coordinates that entity overlaps
        int startX = static_cast<int>(worldX / tileSize);
        int endX = static_cast<int>((worldX + boxWidth) / tileSize);
        int startY = static_cast<int>(worldY / tileSize);
        int endY = static_cast<int>((worldY + boxHeight) / tileSize);

        // Check each tile and resolve collisions
        for (int ty = startY; ty <= endY; ++ty) {
            for (int tx = startX; tx <= endX; ++tx) {
                // Check if tile is in bounds
                if (tx < 0 || tx >= tilemap.getWidth() || ty < 0 || ty >= tilemap.getHeight()) {
                    continue;
                }

                int tileIndex = tilemap.getTile(tx, ty);
                if (tilemap.isSolidTile(tileIndex)) {
                    // Create tile AABB
                    AABB::WorldAABB tileBox{
                        static_cast<float>(tx * tileSize),
                        static_cast<float>(ty * tileSize),
                        static_cast<float>(tileSize),
                        static_cast<float>(tileSize)
                    };

                    AABB::WorldAABB entityBox{worldX, worldY, boxWidth, boxHeight};

                    if (entityBox.intersects(tileBox)) {
                        // Calculate overlap and resolve
                        float overlapX = std::min(entityBox.x + entityBox.width, tileBox.x + tileBox.width)
                                       - std::max(entityBox.x, tileBox.x);
                        float overlapY = std::min(entityBox.y + entityBox.height, tileBox.y + tileBox.height)
                                       - std::max(entityBox.y, tileBox.y);

                        // Push out on smallest overlap axis
                        if (overlapX < overlapY) {
                            if (entityBox.x < tileBox.x) {
                                pos.x -= overlapX;  // Push left
                            } else {
                                pos.x += overlapX;  // Push right
                            }
                        } else {
                            if (entityBox.y < tileBox.y) {
                                pos.y -= overlapY;  // Push up
                            } else {
                                pos.y += overlapY;  // Push down
                            }
                        }

                        // Recalculate for next iteration
                        worldX = pos.x + offsetX;
                        worldY = pos.y + offsetY;
                    }
                }
            }
        }
    }
}

void updateEntityCollisions(entt::registry& registry) {
    auto view = registry.view<Position, AABB, Active>();

    // Simple O(n^2) collision detection - optimize later with spatial partitioning
    for (auto entityA : view) {
        auto& posA = view.get<Position>(entityA);
        auto& aabbA = view.get<AABB>(entityA);

        AABB::WorldAABB boxA{
            posA.x + aabbA.offsetX,
            posA.y + aabbA.offsetY,
            aabbA.width,
            aabbA.height
        };

        for (auto entityB : view) {
            if (entityA == entityB) continue;

            auto& posB = view.get<Position>(entityB);
            auto& aabbB = view.get<AABB>(entityB);

            AABB::WorldAABB boxB{
                posB.x + aabbB.offsetX,
                posB.y + aabbB.offsetY,
                aabbB.width,
                aabbB.height
            };

            if (boxA.intersects(boxB)) {
                // Collision detected - emit signal or handle here
                // For now, just log (can be expanded with collision events)
                // LOG_TRACE("Collision between entity {} and {}", entt::to_integral(entityA), entt::to_integral(entityB));
            }
        }
    }
}

// ============================================================================
// Rendering System
// ============================================================================

void renderSprites(entt::registry& registry, SpriteBatch& batch, Camera& camera) {
    auto view = registry.view<Position, Sprite, Active>();

    for (auto entity : view) {
        auto& pos = view.get<Position>(entity);
        auto& sprite = view.get<Sprite>(entity);

        // Skip if no sprite sheet
        if (!sprite.spriteSheet || sprite.spriteName.empty()) {
            continue;
        }

        const auto* spriteData = sprite.spriteSheet->getSprite(sprite.spriteName);
        if (!spriteData || spriteData->frames.empty()) {
            continue;
        }

        // Get current frame
        int frameIndex = 0;
        if (auto* anim = registry.try_get<Animation>(entity)) {
            frameIndex = anim->currentFrame;
            if (frameIndex >= static_cast<int>(spriteData->frames.size())) {
                frameIndex = 0;
            }
        }

        const SpriteFrame& frame = spriteData->frames[frameIndex];

        // Transform to screen coordinates
        int screenX, screenY;
        camera.worldToScreen(pos.x, pos.y, screenX, screenY);

        // Draw sprite with tint
        batch.draw(sprite.spriteSheet->getTexture(), screenX, screenY, frame,
                   sprite.tintR, sprite.tintG, sprite.tintB, sprite.tintA);
    }
}

// ============================================================================
// Camera System
// ============================================================================

void updateCameraFollow(entt::registry& registry, Camera& camera, float dt) {
    auto view = registry.view<Position, Size, CameraTarget, Active>();

    // Follow first active camera target
    for (auto entity : view) {
        auto& pos = view.get<Position>(entity);
        auto& size = view.get<Size>(entity);
        auto& target = view.get<CameraTarget>(entity);

        // Calculate entity center
        float centerX = pos.x + size.width / 2.0f;
        float centerY = pos.y + size.height / 2.0f;

        // Set camera to follow (using legacy follow method for now)
        // In future, we can implement smooth follow directly here
        camera.setPosition(centerX, centerY);

        // Only follow first target
        break;
    }
}

} // namespace Runa::ECS::Systems
