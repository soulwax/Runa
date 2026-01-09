

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

    if (m_smoothing < 1.0f) {
        float lerpFactor = 1.0f - std::pow(1.0f - m_smoothing, dt * 60.0f);
        m_x += (m_targetX - m_x) * lerpFactor;
        m_y += (m_targetY - m_y) * lerpFactor;
    } else {

        m_x = m_targetX;
        m_y = m_targetY;
    }
}

void Camera::worldToScreen(float worldX, float worldY, int& screenX, int& screenY) const {


    int halfWidth = m_window.getWidth() / 2;
    int halfHeight = m_window.getHeight() / 2;

    screenX = static_cast<int>((worldX - m_x) * m_zoom + halfWidth);
    screenY = static_cast<int>((worldY - m_y) * m_zoom + halfHeight);
}

void Camera::screenToWorld(int screenX, int screenY, float& worldX, float& worldY) const {

    int halfWidth = m_window.getWidth() / 2;
    int halfHeight = m_window.getHeight() / 2;

    worldX = (screenX - halfWidth) / m_zoom + m_x;
    worldY = (screenY - halfHeight) / m_zoom + m_y;
}

Camera::Bounds Camera::getWorldBounds() const {

    int halfWidth = m_window.getWidth() / 2;
    int halfHeight = m_window.getHeight() / 2;

    float worldHalfWidth = halfWidth / m_zoom;
    float worldHalfHeight = halfHeight / m_zoom;

    return Bounds{
        m_x - worldHalfWidth,
        m_y - worldHalfHeight,
        m_x + worldHalfWidth,
        m_y + worldHalfHeight
    };
}

void Camera::handleInput(const Input& input, float dt, float moveSpeed) {

    float moveX = 0.0f;
    float moveY = 0.0f;

    if (input.isKeyDown(SDLK_W)) moveY -= 1.0f;
    if (input.isKeyDown(SDLK_S)) moveY += 1.0f;
    if (input.isKeyDown(SDLK_A)) moveX -= 1.0f;
    if (input.isKeyDown(SDLK_D)) moveX += 1.0f;


    if (moveX != 0.0f && moveY != 0.0f) {
        float length = std::sqrt(moveX * moveX + moveY * moveY);
        moveX /= length;
        moveY /= length;
    }


    if (moveX != 0.0f || moveY != 0.0f) {
        float scaledSpeed = moveSpeed / m_zoom;
        m_x += moveX * scaledSpeed * dt;
        m_y += moveY * scaledSpeed * dt;

        m_targetX = m_x;
        m_targetY = m_y;
    }


    int mouseX, mouseY;
    input.getMousePosition(mouseX, mouseY);

    bool isPanningNow = input.isMouseButtonDown(SDL_BUTTON_MIDDLE) ||
                        input.isMouseButtonDown(SDL_BUTTON_RIGHT);

    if (isPanningNow) {
        if (!m_isPanning) {

            m_isPanning = true;
            m_lastMouseX = mouseX;
            m_lastMouseY = mouseY;
        } else {

            int deltaX = mouseX - m_lastMouseX;
            int deltaY = mouseY - m_lastMouseY;


            float worldDeltaX = deltaX / m_zoom;
            float worldDeltaY = deltaY / m_zoom;

            m_x -= worldDeltaX;
            m_y -= worldDeltaY;


            m_targetX = m_x;
            m_targetY = m_y;

            m_lastMouseX = mouseX;
            m_lastMouseY = mouseY;
        }
    } else {
        m_isPanning = false;
    }


    float wheelDelta = input.getMouseWheel();
    if (wheelDelta != 0.0f) {
        zoom(wheelDelta * 0.1f);
    }
}

void Camera::zoom(float delta) {

    m_zoom += delta;


    if (m_zoom < 0.25f) m_zoom = 0.25f;
    if (m_zoom > 4.0f) m_zoom = 4.0f;
}

}
