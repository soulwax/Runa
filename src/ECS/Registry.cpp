


#include "../runapch.h"
#include "Registry.h"
#include "RPGComponents.h"
#include "../Graphics/SpriteSheet.h"

namespace Runa::ECS {

entt::entity EntityRegistry::createEntity(float x, float y) {
    auto entity = m_registry.create();
    m_registry.emplace<Position>(entity, x, y);
    m_registry.emplace<Active>(entity);
    return entity;
}

entt::entity EntityRegistry::createSpriteEntity(float x, float y,
                                               const SpriteSheet* spriteSheet,
                                               const std::string& spriteName,
                                               float width, float height) {
    auto entity = createEntity(x, y);
    m_registry.emplace<Size>(entity, width, height);
    m_registry.emplace<Velocity>(entity);

    Sprite sprite;
    sprite.spriteSheet = spriteSheet;
    sprite.spriteName = spriteName;
    m_registry.emplace<Sprite>(entity, sprite);

    m_registry.emplace<Animation>(entity);

    return entity;
}

entt::entity EntityRegistry::createPlayer(float x, float y,
                                         const SpriteSheet* spriteSheet,
                                         const std::string& spriteName,
                                         float speed) {
    auto entity = createSpriteEntity(x, y, spriteSheet, spriteName, 14.0f, 14.0f);


    m_registry.emplace<Player>(entity);

    PlayerInput playerInput;
    playerInput.speed = speed;
    m_registry.emplace<PlayerInput>(entity, playerInput);

    CameraTarget cameraTarget;
    cameraTarget.smoothing = 0.15f;
    m_registry.emplace<CameraTarget>(entity, cameraTarget);


    AABB aabb;
    aabb.width = 14.0f;
    aabb.height = 14.0f;
    m_registry.emplace<AABB>(entity, aabb);

    return entity;
}

void EntityRegistry::destroyEntity(entt::entity entity) {
    if (m_registry.valid(entity)) {
        m_registry.destroy(entity);
    }
}

void EntityRegistry::addSprite(entt::entity entity, const SpriteSheet* spriteSheet,
                               const std::string& spriteName) {
    if (!m_registry.valid(entity)) return;

    Sprite sprite;
    sprite.spriteSheet = spriteSheet;
    sprite.spriteName = spriteName;
    m_registry.emplace_or_replace<Sprite>(entity, sprite);
}

void EntityRegistry::addAnimation(entt::entity entity, float frameRate, bool loop) {
    if (!m_registry.valid(entity)) return;

    Animation anim;
    anim.frameRate = frameRate;
    anim.loop = loop;
    m_registry.emplace_or_replace<Animation>(entity, anim);
}

void EntityRegistry::addCollision(entt::entity entity, float width, float height,
                                  float offsetX, float offsetY) {
    if (!m_registry.valid(entity)) return;

    AABB aabb;
    aabb.width = width;
    aabb.height = height;
    aabb.offsetX = offsetX;
    aabb.offsetY = offsetY;
    m_registry.emplace_or_replace<AABB>(entity, aabb);
}

}
