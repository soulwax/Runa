#ifndef ENTITY_H
#define ENTITY_H

#include <string>

/**
 * Entity - Base class for all game entities
 * Provides basic position and identification
 */
class Entity {
public:
    Entity(float x = 0.0f, float y = 0.0f);
    virtual ~Entity() = default;

    // Position
    float getX() const { return m_x; }
    float getY() const { return m_y; }
    void setPosition(float x, float y);
    void move(float dx, float dy);

    // Identification
    const std::string& getName() const { return m_name; }
    void setName(const std::string& name) { m_name = name; }

    // Update (called each frame)
    virtual void update(float deltaTime) {}

    // Render (called each frame)
    virtual void render() {}

protected:
    float m_x = 0.0f;
    float m_y = 0.0f;
    std::string m_name = "Entity";
};

#endif // ENTITY_H
