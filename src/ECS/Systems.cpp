
#include "../runapch.h"
#include "Systems.h"
#include "Components.h"
#include "../Core/Input.h"
#include "../Core/Keybindings.h"
#include "../Graphics/SpriteBatch.h"
#include "../Graphics/Camera.h"
#include "../Graphics/TileMap.h"
#include "../Graphics/Texture.h"
#include "../Collision/CollisionMap.h"
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

        float centerX = pos.x + size.width * 0.5f;
        float centerY = pos.y + size.height * 0.5f;

        camera.setPosition(centerX, centerY);
        break;
    }
}

void updateMapCollision(entt::registry& registry, CollisionMap& collisionMap, float dt,
                        std::function<void(entt::entity, const CollisionEvent&)> onCollision) {
    auto view = registry.view<Position, Velocity, AABB, Collider, Active>();

    for (auto entity : view) {
        auto& pos = view.get<Position>(entity);
        auto& vel = view.get<Velocity>(entity);
        auto& aabb = view.get<AABB>(entity);
        auto& collider = view.get<Collider>(entity);

        if (!collider.enabled || !collider.blocksMovement) {
            continue;
        }

        // Calculate entity's world AABB (position has already been updated by updateMovement)
        float entityX = pos.x + aabb.offsetX;
        float entityY = pos.y + aabb.offsetY;

        // Calculate where the entity was BEFORE movement (to check swept region)
        float prevX = entityX - vel.x * dt;
        float prevY = entityY - vel.y * dt;

        // Check horizontal movement
        if (vel.x != 0.0f) {
            // Check swept region from previous position to current position
            float sweptMinX = std::min(prevX, entityX);
            float sweptMaxX = std::max(prevX + aabb.width, entityX + aabb.width);
            float sweptWidth = sweptMaxX - sweptMinX;

            CollisionType colType = collisionMap.checkMovement(sweptMinX, entityY, sweptWidth, aabb.height);

            // Block movement for Solid, Liquid, or Hazard collision types
            if (colType == CollisionType::Solid || colType == CollisionType::Liquid || colType == CollisionType::Hazard) {
                // Find the exact collision point by binary search
                // We know there's a collision between entityX and testX
                // Find the closest safe position to the collision
                float safeX = entityX;
                const int steps = 20;  // More steps for better precision

                if (vel.x > 0.0f) {
                    // Moving right - search from prevX towards entityX
                    // Find the rightmost position that's still safe
                    float step = (entityX - prevX) / static_cast<float>(steps);
                    for (int i = steps; i >= 0; --i) {
                        float checkX = prevX + step * static_cast<float>(i);
                        if (collisionMap.checkMovement(checkX, entityY, aabb.width, aabb.height) == CollisionType::None) {
                            safeX = checkX;
                            break;
                        }
                    }
                } else {
                    // Moving left - search from prevX towards entityX (entityX < prevX)
                    // Find the leftmost position that's still safe
                    float step = (prevX - entityX) / static_cast<float>(steps);
                    for (int i = steps; i >= 0; --i) {
                        float checkX = prevX - step * static_cast<float>(i);
                        if (collisionMap.checkMovement(checkX, entityY, aabb.width, aabb.height) == CollisionType::None) {
                            safeX = checkX;
                            break;
                        }
                    }
                }

                // Adjust position to safe position (accounting for AABB offset)
                pos.x = safeX - aabb.offsetX;

                // Block horizontal movement
                bool wasMovingLeft = vel.x < 0.0f;
                bool wasMovingRight = vel.x > 0.0f;
                vel.x = 0.0f;

                if (onCollision) {
                    CollisionEvent event;
                    event.fromLeft = wasMovingLeft;
                    event.fromRight = wasMovingRight;
                    onCollision(entity, event);
                }
            }
        }

        // Check vertical movement
        if (vel.y != 0.0f) {
            // Check swept region from previous position to current position
            float sweptMinY = std::min(prevY, entityY);
            float sweptMaxY = std::max(prevY + aabb.height, entityY + aabb.height);
            float sweptHeight = sweptMaxY - sweptMinY;

            CollisionType colType = collisionMap.checkMovement(entityX, sweptMinY, aabb.width, sweptHeight);

            // Block movement for Solid, Liquid, or Hazard collision types
            if (colType == CollisionType::Solid || colType == CollisionType::Liquid || colType == CollisionType::Hazard) {
                // Find the exact collision point by binary search
                float safeY = entityY;
                const int steps = 20;  // More steps for better precision

                if (vel.y > 0.0f) {
                    // Moving down - search from prevY towards entityY
                    float step = (entityY - prevY) / static_cast<float>(steps);
                    for (int i = steps; i >= 0; --i) {
                        float checkY = prevY + step * static_cast<float>(i);
                        if (collisionMap.checkMovement(entityX, checkY, aabb.width, aabb.height) == CollisionType::None) {
                            safeY = checkY;
                            break;
                        }
                    }
                } else {
                    // Moving up - search from prevY towards entityY (entityY < prevY)
                    float step = (prevY - entityY) / static_cast<float>(steps);
                    for (int i = steps; i >= 0; --i) {
                        float checkY = prevY - step * static_cast<float>(i);
                        if (collisionMap.checkMovement(entityX, checkY, aabb.width, aabb.height) == CollisionType::None) {
                            safeY = checkY;
                            break;
                        }
                    }
                }

                // Adjust position to safe position (accounting for AABB offset)
                pos.y = safeY - aabb.offsetY;

                // Block vertical movement
                bool wasMovingUp = vel.y < 0.0f;
                bool wasMovingDown = vel.y > 0.0f;
                vel.y = 0.0f;

                if (onCollision) {
                    CollisionEvent event;
                    event.fromTop = wasMovingUp;
                    event.fromBottom = wasMovingDown;
                    onCollision(entity, event);
                }
            }
        }

        // Also check current position (in case we're already overlapping)
        CollisionType currentCol = collisionMap.checkMovement(entityX, entityY, aabb.width, aabb.height);
        if (currentCol == CollisionType::Solid || currentCol == CollisionType::Liquid || currentCol == CollisionType::Hazard) {
            // Push out of collision - simple resolution
            // This handles the case where we're already inside a tile
            if (vel.x == 0.0f && vel.y == 0.0f) {
                // Try to push out in the direction we came from
                // For now, just zero velocity (we're stuck)
            }
        }
    }
}

void updateEntityToEntityCollision(entt::registry& registry,
                                   std::function<void(entt::entity, entt::entity, const CollisionEvent&)> onCollision) {
    auto view = registry.view<Position, AABB, Collider, Active>();

    std::vector<entt::entity> entities(view.begin(), view.end());

    for (size_t i = 0; i < entities.size(); ++i) {
        auto entityA = entities[i];
        auto& posA = view.get<Position>(entityA);
        auto& aabbA = view.get<AABB>(entityA);
        auto& colliderA = view.get<Collider>(entityA);

        if (!colliderA.enabled) continue;

        float ax = posA.x + aabbA.offsetX;
        float ay = posA.y + aabbA.offsetY;

        for (size_t j = i + 1; j < entities.size(); ++j) {
            auto entityB = entities[j];
            auto& posB = view.get<Position>(entityB);
            auto& aabbB = view.get<AABB>(entityB);
            auto& colliderB = view.get<Collider>(entityB);

            if (!colliderB.enabled) continue;

            float bx = posB.x + aabbB.offsetX;
            float by = posB.y + aabbB.offsetY;

            // AABB intersection test
            bool overlaps = !(ax + aabbA.width <= bx ||
                              ax >= bx + aabbB.width ||
                              ay + aabbA.height <= by ||
                              ay >= by + aabbB.height);

            if (overlaps) {
                // Calculate overlap
                float overlapX = std::min(ax + aabbA.width, bx + aabbB.width) - std::max(ax, bx);
                float overlapY = std::min(ay + aabbA.height, by + aabbB.height) - std::max(ay, by);

                CollisionEvent event;
                event.other = entityB;
                event.overlapX = overlapX;
                event.overlapY = overlapY;
                event.fromLeft = ax < bx;
                event.fromRight = ax > bx;
                event.fromTop = ay < by;
                event.fromBottom = ay > by;

                if (onCollision) {
                    onCollision(entityA, entityB, event);
                }

                // Resolve collision if both are solid
                if (colliderA.blocksMovement && colliderB.blocksMovement) {
                    // Push entities apart (simple resolution)
                    if (overlapX < overlapY) {
                        float pushX = overlapX * 0.5f;
                        if (ax < bx) {
                            posA.x -= pushX;
                            posB.x += pushX;
                        } else {
                            posA.x += pushX;
                            posB.x -= pushX;
                        }
                    } else {
                        float pushY = overlapY * 0.5f;
                        if (ay < by) {
                            posA.y -= pushY;
                            posB.y += pushY;
                        } else {
                            posA.y += pushY;
                            posB.y -= pushY;
                        }
                    }
                }
            }
        }
    }
}

void updateInteraction(entt::registry& registry, Input& input, int interactionKey,
                       std::function<void(entt::entity, entt::entity, Interactable&)> onInteract) {
    // Find entities that can interact (usually the player)
    auto interactors = registry.view<Position, Size, CanInteract, Active>();
    auto interactables = registry.view<Position, Size, Interactable, Active>();

    for (auto interactor : interactors) {
        auto& interactorPos = interactors.get<Position>(interactor);
        auto& interactorSize = interactors.get<Size>(interactor);
        auto& canInteract = interactors.get<CanInteract>(interactor);

        // Center of interactor
        float ix = interactorPos.x + interactorSize.width * 0.5f;
        float iy = interactorPos.y + interactorSize.height * 0.5f;

        // Check if interaction key is pressed
        bool keyPressed = input.isKeyPressed(static_cast<SDL_Keycode>(interactionKey));

        if (!keyPressed) continue;

        // Find closest interactable in range
        entt::entity closest = entt::null;
        float closestDist = canInteract.range * canInteract.range;
        Interactable* closestInteractable = nullptr;

        for (auto target : interactables) {
            if (target == interactor) continue;

            auto& targetPos = interactables.get<Position>(target);
            auto& targetSize = interactables.get<Size>(target);
            auto& interactable = interactables.get<Interactable>(target);

            if (interactable.consumed) continue;

            // Center of target
            float tx = targetPos.x + targetSize.width * 0.5f;
            float ty = targetPos.y + targetSize.height * 0.5f;

            float dx = tx - ix;
            float dy = ty - iy;
            float distSq = dx * dx + dy * dy;

            if (distSq < closestDist && distSq <= interactable.interactionRange * interactable.interactionRange) {
                closest = target;
                closestDist = distSq;
                closestInteractable = &interactable;
            }
        }

        if (closest != entt::null && closestInteractable && onInteract) {
            onInteract(interactor, closest, *closestInteractable);

            if (closestInteractable->oneTime) {
                closestInteractable->consumed = true;
            }
        }
    }
}

void updateTileInteraction(entt::registry& registry, CollisionMap& collisionMap,
                           Input& input, int interactionKey,
                           std::function<void(entt::entity, TileInteraction&)> onInteract) {
    auto view = registry.view<Position, Size, CanInteract, Active>();

    bool keyPressed = input.isKeyPressed(static_cast<SDL_Keycode>(interactionKey));
    if (!keyPressed) return;

    for (auto entity : view) {
        auto& pos = view.get<Position>(entity);
        auto& size = view.get<Size>(entity);
        auto& canInteract = view.get<CanInteract>(entity);

        // Center of entity
        float cx = pos.x + size.width * 0.5f;
        float cy = pos.y + size.height * 0.5f;

        // Get tile interactions in range
        auto interactions = collisionMap.getInteractionsInRange(cx, cy, canInteract.range);

        for (TileInteraction* interaction : interactions) {
            if (interaction && !interaction->consumed && onInteract) {
                onInteract(entity, *interaction);

                if (interaction->oneTime) {
                    interaction->consumed = true;
                }
                break;  // Only interact with one tile at a time
            }
        }
    }
}

std::vector<entt::entity> getInteractablesInRange(entt::registry& registry,
                                                   entt::entity source, float range) {
    std::vector<entt::entity> result;

    auto* sourcePos = registry.try_get<Position>(source);
    auto* sourceSize = registry.try_get<Size>(source);

    if (!sourcePos || !sourceSize) return result;

    float sx = sourcePos->x + sourceSize->width * 0.5f;
    float sy = sourcePos->y + sourceSize->height * 0.5f;
    float rangeSq = range * range;

    auto view = registry.view<Position, Size, Interactable, Active>();

    for (auto target : view) {
        if (target == source) continue;

        auto& targetPos = view.get<Position>(target);
        auto& targetSize = view.get<Size>(target);
        auto& interactable = view.get<Interactable>(target);

        if (interactable.consumed) continue;

        float tx = targetPos.x + targetSize.width * 0.5f;
        float ty = targetPos.y + targetSize.height * 0.5f;

        float dx = tx - sx;
        float dy = ty - sy;
        float distSq = dx * dx + dy * dy;

        if (distSq <= rangeSq) {
            result.push_back(target);
        }
    }

    return result;
}

}
