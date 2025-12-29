// File: src/Core/Entity.cpp

#include "runapch.h"
#include "Entity.h"
#include "Graphics/SpriteBatch.h"

namespace Runa {

// AABB implementation
bool AABB::intersects(const AABB& other) const {
    return (x < other.x + other.width &&
            x + width > other.x &&
            y < other.y + other.height &&
            y + height > other.y);
}

bool AABB::contains(float px, float py) const {
    return (px >= x && px < x + width &&
            py >= y && py < y + height);
}

// Entity implementation
Entity::Entity(float x, float y, float width, float height)
    : m_x(x), m_y(y), m_width(width), m_height(height),
      m_velocityX(0.0f), m_velocityY(0.0f)
{
}

void Entity::update(float dt) {
    // Base implementation: apply velocity to position
    m_x += m_velocityX * dt;
    m_y += m_velocityY * dt;

    // Update animation time
    m_animationTime += dt;
}

void Entity::render(SpriteBatch& batch) {
    if (!m_spriteSheet || m_spriteName.empty()) {
        return;
    }

    const Sprite* sprite = m_spriteSheet->getSprite(m_spriteName);
    if (!sprite || sprite->frames.empty()) {
        return;
    }

    // Get current frame (simple frame cycling)
    if (sprite->frames.size() > 1) {
        // Cycle through frames at 10 FPS
        m_currentFrame = static_cast<int>(m_animationTime * 10.0f) % static_cast<int>(sprite->frames.size());
    } else {
        m_currentFrame = 0;
    }

    const SpriteFrame& frame = sprite->frames[m_currentFrame];

    // Render sprite at entity position
    batch.draw(m_spriteSheet->getTexture(),
               static_cast<int>(m_x), static_cast<int>(m_y),
               frame);
}

void Entity::move(float dx, float dy) {
    m_x += dx;
    m_y += dy;
}

void Entity::setPosition(float x, float y) {
    m_x = x;
    m_y = y;
}

void Entity::setVelocity(float vx, float vy) {
    m_velocityX = vx;
    m_velocityY = vy;
}

AABB Entity::getAABB() const {
    return AABB{m_x, m_y, m_width, m_height};
}

bool Entity::collidesWith(const Entity& other) const {
    return getAABB().intersects(other.getAABB());
}

void Entity::setSprite(const SpriteSheet* spriteSheet, const std::string& spriteName) {
    m_spriteSheet = spriteSheet;
    m_spriteName = spriteName;
    m_currentFrame = 0;
    m_animationTime = 0.0f;
}

} // namespace Runa
