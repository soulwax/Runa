// File: src/Graphics/Camera.h

#ifndef RUNA_GRAPHICS_CAMERA_H
#define RUNA_GRAPHICS_CAMERA_H

#include "../RunaAPI.h"
#include "../Core/Entity.h"
#include <entt/fwd.hpp>

namespace Runa {

class Window;
class Input;

// 2D camera for world-to-screen transformation
class RUNA_API Camera {
public:
    explicit Camera(Window& window);
    ~Camera() = default;

    // Set camera position (center of view)
    void setPosition(float x, float y);
    void move(float dx, float dy);

    // Follow an entity (smooth camera) - legacy method
    void follow(const Entity& entity, float smoothing = 1.0f);

    // Follow an entity via EnTT registry (new ECS method)
    void followEntity(entt::registry& registry, entt::entity entity, float smoothing = 1.0f);

    void update(float dt);

    // Manual camera controls
    void handleInput(const Input& input, float dt, float moveSpeed = 300.0f);
    void zoom(float delta);  // Zoom in/out by delta

    // World-to-screen coordinate transformation
    void worldToScreen(float worldX, float worldY, int& screenX, int& screenY) const;
    void screenToWorld(int screenX, int screenY, float& worldX, float& worldY) const;

    // Get camera bounds in world space
    struct Bounds {
        float left, top, right, bottom;
    };
    Bounds getWorldBounds() const;

    // Zoom (for future)
    void setZoom(float zoom) { 
        m_zoom = (zoom > 0.001f) ? zoom : 0.001f;  // Prevent division by zero
        if (m_zoom < 0.25f) m_zoom = 0.25f;
        if (m_zoom > 4.0f) m_zoom = 4.0f;
    }
    float getZoom() const { return m_zoom; }

    // Getters
    float getX() const { return m_x; }
    float getY() const { return m_y; }

private:
    Window& m_window;
    float m_x = 0.0f;        // Camera center X
    float m_y = 0.0f;        // Camera center Y
    float m_zoom = 1.0f;
    float m_targetX = 0.0f;  // For smooth following
    float m_targetY = 0.0f;
    float m_smoothing = 1.0f; // 1.0 = instant, 0.1 = smooth

    // Mouse panning state
    bool m_isPanning = false;
    int m_lastMouseX = 0;
    int m_lastMouseY = 0;
};

} // namespace Runa

#endif // RUNA_GRAPHICS_CAMERA_H
