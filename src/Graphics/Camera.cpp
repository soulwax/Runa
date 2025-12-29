// File: src/Graphics/Camera.cpp

#include "../runapch.h"
#include "Camera.h"
#include "Window.h"

namespace Runa {

Camera::Camera(Window& window)
    : m_window(window)
{
}

void Camera::setPosition(float x, float y) {
    m_x = x;
    m_y = y;
    m_targetX = x;
    m_targetY = y;
}

void Camera::move(float dx, float dy) {
    m_x += dx;
    m_y += dy;
    m_targetX = m_x;
    m_targetY = m_y;
}

void Camera::follow(const Entity& entity, float smoothing) {
    // Set target to entity center
    m_targetX = entity.getX() + entity.getWidth() / 2.0f;
    m_targetY = entity.getY() + entity.getHeight() / 2.0f;
    m_smoothing = smoothing;
}

void Camera::update(float dt) {
    // Smooth camera follow using lerp
    if (m_smoothing < 1.0f) {
        float lerpFactor = 1.0f - std::pow(1.0f - m_smoothing, dt * 60.0f);  // Frame-rate independent
        m_x += (m_targetX - m_x) * lerpFactor;
        m_y += (m_targetY - m_y) * lerpFactor;
    } else {
        // Instant follow
        m_x = m_targetX;
        m_y = m_targetY;
    }
}

void Camera::worldToScreen(float worldX, float worldY, int& screenX, int& screenY) const {
    // Transform from world coordinates to screen coordinates
    // Camera position is the center of the view
    int halfWidth = m_window.getWidth() / 2;
    int halfHeight = m_window.getHeight() / 2;

    screenX = static_cast<int>((worldX - m_x) * m_zoom + halfWidth);
    screenY = static_cast<int>((worldY - m_y) * m_zoom + halfHeight);
}

void Camera::screenToWorld(int screenX, int screenY, float& worldX, float& worldY) const {
    // Transform from screen coordinates to world coordinates
    int halfWidth = m_window.getWidth() / 2;
    int halfHeight = m_window.getHeight() / 2;

    worldX = (screenX - halfWidth) / m_zoom + m_x;
    worldY = (screenY - halfHeight) / m_zoom + m_y;
}

Camera::Bounds Camera::getWorldBounds() const {
    // Calculate visible area in world space
    int halfWidth = m_window.getWidth() / 2;
    int halfHeight = m_window.getHeight() / 2;

    float worldHalfWidth = halfWidth / m_zoom;
    float worldHalfHeight = halfHeight / m_zoom;

    return Bounds{
        m_x - worldHalfWidth,   // left
        m_y - worldHalfHeight,  // top
        m_x + worldHalfWidth,   // right
        m_y + worldHalfHeight   // bottom
    };
}

} // namespace Runa
