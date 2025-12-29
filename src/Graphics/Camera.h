// File: src/Graphics/Camera.h

#ifndef RUNA_GRAPHICS_CAMERA_H
#define RUNA_GRAPHICS_CAMERA_H

#include "RunaAPI.h"
#include "Core/Entity.h"

namespace Runa {

class Window;

// 2D camera for world-to-screen transformation
class RUNA_API Camera {
public:
    explicit Camera(Window& window);
    ~Camera() = default;

    // Set camera position (center of view)
    void setPosition(float x, float y);
    void move(float dx, float dy);

    // Follow an entity (smooth camera)
    void follow(const Entity& entity, float smoothing = 1.0f);
    void update(float dt);

    // World-to-screen coordinate transformation
    void worldToScreen(float worldX, float worldY, int& screenX, int& screenY) const;
    void screenToWorld(int screenX, int screenY, float& worldX, float& worldY) const;

    // Get camera bounds in world space
    struct Bounds {
        float left, top, right, bottom;
    };
    Bounds getWorldBounds() const;

    // Zoom (for future)
    void setZoom(float zoom) { m_zoom = zoom; }
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
};

} // namespace Runa

#endif // RUNA_GRAPHICS_CAMERA_H
