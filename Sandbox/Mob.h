// File: Sandbox/Mob.h

#ifndef MOB_H
#define MOB_H

#include "Entity.h"

/**
 * Mob - Mobile entity (can move)
 * Inherits from Entity and adds movement capabilities
 */
class Mob : public Entity {
public:
    Mob(float x = 0.0f, float y = 0.0f);
    virtual ~Mob() = default;

    // Movement
    float getSpeed() const { return m_speed; }
    void setSpeed(float speed) { m_speed = speed; }

    // Velocity
    float getVelocityX() const { return m_velocityX; }
    float getVelocityY() const { return m_velocityY; }
    void setVelocity(float vx, float vy);
    void addVelocity(float vx, float vy);

    // Update movement
    void update(float deltaTime) override;

protected:
    float m_speed = 100.0f;  // Default speed in pixels per second
    float m_velocityX = 0.0f;
    float m_velocityY = 0.0f;
};

#endif // MOB_H
