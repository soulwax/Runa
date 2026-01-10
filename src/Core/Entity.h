// File: src/Core/Entity.h

#ifndef RUNA_CORE_ENTITY_H
#define RUNA_CORE_ENTITY_H

#include "../RunaAPI.h"
#include "Graphics/SpriteSheet.h"
#include <memory>
#include <string>

namespace Runa {

class SpriteBatch;


struct RUNA_API AABB {
    float x, y;
    float width, height;

    bool intersects(const AABB& other) const;
    bool contains(float px, float py) const;
};


class RUNA_API Entity {
public:
    Entity(float x, float y, float width, float height);
    virtual ~Entity() = default;


    virtual void update(float dt);


    virtual void render(SpriteBatch& batch);


    void move(float dx, float dy);
    void setPosition(float x, float y);
    void setVelocity(float vx, float vy);


    AABB getAABB() const;
    bool collidesWith(const Entity& other) const;


    void setSprite(const SpriteSheet* spriteSheet, const std::string& spriteName);


    float getX() const { return m_x; }
    float getY() const { return m_y; }
    float getWidth() const { return m_width; }
    float getHeight() const { return m_height; }
    float getVelocityX() const { return m_velocityX; }
    float getVelocityY() const { return m_velocityY; }


    bool isActive() const { return m_active; }
    void setActive(bool active) { m_active = active; }

protected:
    float m_x, m_y;
    float m_width, m_height;
    float m_velocityX, m_velocityY;
    bool m_active = true;


    const SpriteSheet* m_spriteSheet = nullptr;
    std::string m_spriteName;
    int m_currentFrame = 0;
    float m_animationTime = 0.0f;
};

}

#endif
