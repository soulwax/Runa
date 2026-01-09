

#ifndef RUNA_GRAPHICS_CAMERA_H
#define RUNA_GRAPHICS_CAMERA_H

#include "../RunaAPI.h"
#include "../Core/Entity.h"
#include <entt/fwd.hpp>

namespace Runa {

class Window;
class Input;


class RUNA_API Camera {
public:
    explicit Camera(Window& window);
    ~Camera() = default;


    void setPosition(float x, float y);
    void move(float dx, float dy);


    void follow(const Entity& entity, float smoothing = 1.0f);


    void followEntity(entt::registry& registry, entt::entity entity, float smoothing = 1.0f);

    void update(float dt);


    void handleInput(const Input& input, float dt, float moveSpeed = 300.0f);
    void zoom(float delta);


    void worldToScreen(float worldX, float worldY, int& screenX, int& screenY) const;
    void screenToWorld(int screenX, int screenY, float& worldX, float& worldY) const;


    struct Bounds {
        float left, top, right, bottom;
    };
    Bounds getWorldBounds() const;


    void setZoom(float zoom) {
        m_zoom = (zoom > 0.001f) ? zoom : 0.001f;
        if (m_zoom < 0.25f) m_zoom = 0.25f;
        if (m_zoom > 4.0f) m_zoom = 4.0f;
    }
    float getZoom() const { return m_zoom; }


    float getX() const { return m_x; }
    float getY() const { return m_y; }

private:
    Window& m_window;
    float m_x = 0.0f;
    float m_y = 0.0f;
    float m_zoom = 1.0f;
    float m_targetX = 0.0f;
    float m_targetY = 0.0f;
    float m_smoothing = 1.0f;


    bool m_isPanning = false;
    int m_lastMouseX = 0;
    int m_lastMouseY = 0;
};

}

#endif
