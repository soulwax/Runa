// File: src/Graphics/Camera.cpp

#include "../runapch.h"
#include "Camera.h"
#include "Window.h"
#include "../Core/Input.h"
#include "../ECS/Components.h"
#include <entt/entt.hpp>

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

void Camera::followEntity(entt::registry& registry, entt::entity entity, float smoothing) {
    if (!registry.valid(entity)) return;

    auto* pos = registry.try_get<ECS::Position>(entity);
    auto* size = registry.try_get<ECS::Size>(entity);

    if (pos && size) {
        m_targetX = pos->x + size->width / 2.0f;
        m_targetY = pos->y + size->height / 2.0f;
        m_smoothing = smoothing;
    }
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

void Camera::handleInput(const Input& input, float dt, float moveSpeed) {
    // WASD camera movement (overrides smooth following)
    float moveX = 0.0f;
    float moveY = 0.0f;

    if (input.isKeyDown(SDLK_W)) moveY -= 1.0f;
    if (input.isKeyDown(SDLK_S)) moveY += 1.0f;
    if (input.isKeyDown(SDLK_A)) moveX -= 1.0f;
    if (input.isKeyDown(SDLK_D)) moveX += 1.0f;

    // Normalize diagonal movement
    if (moveX != 0.0f && moveY != 0.0f) {
        float length = std::sqrt(moveX * moveX + moveY * moveY);
        moveX /= length;
        moveY /= length;
    }

    // Apply WASD movement (adjust speed by zoom for consistent feel)
    if (moveX != 0.0f || moveY != 0.0f) {
        float scaledSpeed = moveSpeed / m_zoom;
        m_x += moveX * scaledSpeed * dt;
        m_y += moveY * scaledSpeed * dt;
        // Update target to prevent smooth follow from fighting us
        m_targetX = m_x;
        m_targetY = m_y;
    }

    // Mouse drag panning (middle button or right button)
    int mouseX, mouseY;
    input.getMousePosition(mouseX, mouseY);

    bool isPanningNow = input.isMouseButtonDown(SDL_BUTTON_MIDDLE) ||
                        input.isMouseButtonDown(SDL_BUTTON_RIGHT);

    if (isPanningNow) {
        if (!m_isPanning) {
            // Just started panning
            m_isPanning = true;
            m_lastMouseX = mouseX;
            m_lastMouseY = mouseY;
        } else {
            // Continue panning - move camera opposite to mouse movement
            int deltaX = mouseX - m_lastMouseX;
            int deltaY = mouseY - m_lastMouseY;

            // Convert screen-space delta to world-space delta
            float worldDeltaX = deltaX / m_zoom;
            float worldDeltaY = deltaY / m_zoom;

            m_x -= worldDeltaX;
            m_y -= worldDeltaY;

            // Update target to prevent smooth follow from fighting us
            m_targetX = m_x;
            m_targetY = m_y;

            m_lastMouseX = mouseX;
            m_lastMouseY = mouseY;
        }
    } else {
        m_isPanning = false;
    }

    // Mouse wheel zoom
    float wheelDelta = input.getMouseWheel();
    if (wheelDelta != 0.0f) {
        zoom(wheelDelta * 0.1f);  // Scale wheel delta for smoother zoom
    }
}

void Camera::zoom(float delta) {
    // Apply zoom delta
    m_zoom += delta;

    // Clamp zoom to reasonable values
    if (m_zoom < 0.25f) m_zoom = 0.25f;  // Min zoom out (4x world view)
    if (m_zoom > 4.0f) m_zoom = 4.0f;    // Max zoom in (4x magnification)
}

} // namespace Runa
