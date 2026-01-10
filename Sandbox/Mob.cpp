#include "Mob.h"

Mob::Mob(float x, float y) : Entity(x, y) {
}

void Mob::setVelocity(float vx, float vy) {
    m_velocityX = vx;
    m_velocityY = vy;
}

void Mob::addVelocity(float vx, float vy) {
    m_velocityX += vx;
    m_velocityY += vy;
}

void Mob::update(float deltaTime) {
    Entity::update(deltaTime);

    // Apply velocity to position
    if (m_velocityX != 0.0f || m_velocityY != 0.0f) {
        move(m_velocityX * deltaTime, m_velocityY * deltaTime);
    }
}
