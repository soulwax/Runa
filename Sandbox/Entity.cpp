#include "Entity.h"

Entity::Entity(float x, float y) : m_x(x), m_y(y) {
}

void Entity::setPosition(float x, float y) {
    m_x = x;
    m_y = y;
}

void Entity::move(float dx, float dy) {
    m_x += dx;
    m_y += dy;
}
