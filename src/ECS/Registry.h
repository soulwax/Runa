


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


class RUNA_API EntityRegistry {
public:
    EntityRegistry() = default;
    ~EntityRegistry() = default;


    entt::registry& getRegistry() { return m_registry; }
    const entt::registry& getRegistry() const { return m_registry; }






    entt::entity createEntity(float x = 0.0f, float y = 0.0f);


    entt::entity createSpriteEntity(float x, float y,
                                   const SpriteSheet* spriteSheet,
                                   const std::string& spriteName,
                                   float width = 16.0f, float height = 16.0f);


    entt::entity createPlayer(float x, float y,
                              const SpriteSheet* spriteSheet,
                              const std::string& spriteName,
                              float speed = 120.0f);


    void destroyEntity(entt::entity entity);






    void addSprite(entt::entity entity, const SpriteSheet* spriteSheet,
                   const std::string& spriteName);


    void addAnimation(entt::entity entity, float frameRate = 10.0f, bool loop = true);


    void addCollision(entt::entity entity, float width, float height,
                      float offsetX = 0.0f, float offsetY = 0.0f);







    size_t getEntityCount() const {
        size_t count = 0;
        for (auto entity : m_registry.view<Active>()) {
            (void)entity;
            ++count;
        }
        return count;
    }


    void clear() { m_registry.clear(); }

private:
    entt::registry m_registry;
};

}

#endif
