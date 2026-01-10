
#include "../runapch.h"
#include "Systems.h"
#include "Components.h"
#include "../Core/Input.h"
#include "../Core/Keybindings.h"
#include "../Graphics/SpriteBatch.h"
#include "../Graphics/Camera.h"
#include "../Graphics/TileMap.h"
#include "../Graphics/Texture.h"
#include "../Core/Log.h"
#include <cmath>
#include <algorithm>

namespace Runa::ECS::Systems {

// Helper function to check if any key in a list is down
static bool isAnyKeyDown(const Input& input, const std::vector<SDL_Keycode>& keys) {
    for (SDL_Keycode key : keys) {
        if (input.isKeyDown(key)) {
            return true;
        }
    }
    return false;
}

void updatePlayerInput(entt::registry& registry, Input& input, float dt) {
    // Call overloaded version with no keybindings (uses default hardcoded keys)
    updatePlayerInput(registry, input, dt, nullptr);
}

void updatePlayerInput(entt::registry& registry, Input& input, float dt, Keybindings* keybindings) {
    auto view = registry.view<PlayerInput, Velocity, Active>();

    for (auto entity : view) {
        auto& playerInput = view.get<PlayerInput>(entity);
        auto& velocity = view.get<Velocity>(entity);


        float moveX = 0.0f;
        float moveY = 0.0f;

        if (keybindings && keybindings->hasAction("move_left")) {
            // Use keybindings from metadata file
            if (isAnyKeyDown(input, keybindings->getKeys("move_left"))) {
                moveX -= 1.0f;
            }
            if (isAnyKeyDown(input, keybindings->getKeys("move_right"))) {
                moveX += 1.0f;
            }
            if (isAnyKeyDown(input, keybindings->getKeys("move_up"))) {
                moveY -= 1.0f;
            }
            if (isAnyKeyDown(input, keybindings->getKeys("move_down"))) {
                moveY += 1.0f;
            }
        } else {
            // Fallback to hardcoded keys if keybindings not available
            if (input.isKeyDown(SDLK_LEFT) || input.isKeyDown(SDLK_A)) {
                moveX -= 1.0f;
            }
            if (input.isKeyDown(SDLK_RIGHT) || input.isKeyDown(SDLK_D)) {
                moveX += 1.0f;
            }
            if (input.isKeyDown(SDLK_UP) || input.isKeyDown(SDLK_W)) {
                moveY -= 1.0f;
            }
            if (input.isKeyDown(SDLK_DOWN) || input.isKeyDown(SDLK_S)) {
                moveY += 1.0f;
            }
        }


        if (moveX != 0.0f && moveY != 0.0f) {
            float length = std::sqrt(moveX * moveX + moveY * moveY);
            moveX /= length;
            moveY /= length;
        }


        velocity.x = moveX * playerInput.speed;
        velocity.y = moveY * playerInput.speed;
    }
}

void updateMovement(entt::registry& registry, float dt) {
    auto view = registry.view<Position, Velocity, Active>();

    for (auto entity : view) {
        auto& pos = view.get<Position>(entity);
        auto& vel = view.get<Velocity>(entity);


        pos.x += vel.x * dt;
        pos.y += vel.y * dt;
    }
}


void updateAnimation(entt::registry& registry, float dt) {
    auto view = registry.view<Sprite, Animation, Active>();

    for (auto entity : view) {
        auto& sprite = view.get<Sprite>(entity);
        auto& anim = view.get<Animation>(entity);


        if (!sprite.spriteSheet || sprite.spriteName.empty()) {
            continue;
        }

        const auto* spriteData = sprite.spriteSheet->getSprite(sprite.spriteName);
        if (!spriteData || spriteData->frames.empty()) {
            continue;
        }


        anim.animationTime += dt;


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


void updateTileCollisions(entt::registry& registry, const TileMap& tilemap, int tileSize) {
    auto view = registry.view<Position, Size, Active>();

    for (auto entity : view) {
        auto& pos = view.get<Position>(entity);
        auto& size = view.get<Size>(entity);


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


        float worldX = pos.x + offsetX;
        float worldY = pos.y + offsetY;


        int startX = static_cast<int>(worldX / tileSize);
        int endX = static_cast<int>((worldX + boxWidth) / tileSize);
        int startY = static_cast<int>(worldY / tileSize);
        int endY = static_cast<int>((worldY + boxHeight) / tileSize);


        for (int ty = startY; ty <= endY; ++ty) {
            for (int tx = startX; tx <= endX; ++tx) {

                if (tx < 0 || tx >= tilemap.getWidth() || ty < 0 || ty >= tilemap.getHeight()) {
                    continue;
                }

                int tileIndex = tilemap.getTile(tx, ty);
                if (tilemap.isSolidTile(tileIndex)) {

                    AABB::WorldAABB tileBox{
                        static_cast<float>(tx * tileSize),
                        static_cast<float>(ty * tileSize),
                        static_cast<float>(tileSize),
                        static_cast<float>(tileSize)
                    };

                    AABB::WorldAABB entityBox{worldX, worldY, boxWidth, boxHeight};

                    if (entityBox.intersects(tileBox)) {

                        float overlapX = std::min(entityBox.x + entityBox.width, tileBox.x + tileBox.width)
                                       - std::max(entityBox.x, tileBox.x);
                        float overlapY = std::min(entityBox.y + entityBox.height, tileBox.y + tileBox.height)
                                       - std::max(entityBox.y, tileBox.y);


                        if (overlapX < overlapY) {
                            if (entityBox.x < tileBox.x) {
                                pos.x -= overlapX;
                            } else {
                                pos.x += overlapX;
                            }
                        } else {
                            if (entityBox.y < tileBox.y) {
                                pos.y -= overlapY;
                            } else {
                                pos.y += overlapY;
                            }
                        }


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



            }
        }
    }
}





void renderSprites(entt::registry& registry, SpriteBatch& batch, Camera& camera, Texture* whitePixelTexture) {
    auto view = registry.view<Position, Sprite, Active>();

    for (auto entity : view) {
        auto& pos = view.get<Position>(entity);
        auto& sprite = view.get<Sprite>(entity);


        int screenX, screenY;
        camera.worldToScreen(pos.x, pos.y, screenX, screenY);


        float width = 32.0f;
        float height = 32.0f;
        if (auto* size = registry.try_get<Size>(entity)) {
            width = size->width;
            height = size->height;
        }


        if (sprite.spriteSheet && !sprite.spriteName.empty()) {
            const auto* spriteData = sprite.spriteSheet->getSprite(sprite.spriteName);
            if (spriteData && !spriteData->frames.empty()) {

                int frameIndex = 0;
                if (auto* anim = registry.try_get<Animation>(entity)) {
                    frameIndex = anim->currentFrame;
                    if (frameIndex >= static_cast<int>(spriteData->frames.size())) {
                        frameIndex = 0;
                    }
                }

                const SpriteFrame& frame = spriteData->frames[frameIndex];
                float pixelScale = SpriteBatch::getPixelScale();
                // Sprite rendered size is frame size * pixelScale (zoom only affects world-to-screen, not sprite size)
                float spriteWidthPixels = frame.width * pixelScale;
                float spriteHeightPixels = frame.height * pixelScale;

                // Simple centering - flip compensation is handled in SpriteBatch
                float halfWidth = spriteWidthPixels * 0.5f;
                float halfHeight = spriteHeightPixels * 0.5f;
                int drawX = static_cast<int>(screenX - halfWidth);
                int drawY = static_cast<int>(screenY - halfHeight);


                batch.draw(sprite.spriteSheet->getTexture(), drawX, drawY, frame,
                           sprite.tintR, sprite.tintG, sprite.tintB, sprite.tintA,
                           1.0f, 1.0f, sprite.flipX, sprite.flipY);
                continue;
            }
        }


        if (whitePixelTexture && whitePixelTexture->isValid()) {
            float pixelScale = SpriteBatch::getPixelScale();
            // Rendered size is width * pixelScale (zoom only affects world-to-screen)
            float fallbackWidthPixels = width * pixelScale;
            float fallbackHeightPixels = height * pixelScale;
            int drawX = screenX - static_cast<int>(fallbackWidthPixels * 0.5f);
            int drawY = screenY - static_cast<int>(fallbackHeightPixels * 0.5f);

            batch.draw(*whitePixelTexture, drawX, drawY, 0, 0, 1, 1,
                       sprite.tintR, sprite.tintG, sprite.tintB, sprite.tintA,
                       width / 3.0f, height / 3.0f);
        }
    }
}





void updateCameraFollow(entt::registry& registry, Camera& camera, float dt) {
    auto view = registry.view<Position, Size, CameraTarget, Active>();


    for (auto entity : view) {
        auto& pos = view.get<Position>(entity);
        auto& size = view.get<Size>(entity);
        auto& target = view.get<CameraTarget>(entity);


        float centerX = pos.x + size.width / 2.0f;
        float centerY = pos.y + size.height / 2.0f;


        camera.setPosition(centerX, centerY);
        break;
    }
}

}
