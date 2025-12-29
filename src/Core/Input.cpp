// File: src/Core/Input.cpp

#include "../runapch.h"
#include "Input.h"
#include "Graphics/Window.h"

namespace Runa {

Input::Input(Window& window)
    : m_window(window)
{
}

void Input::beginFrame() {
    // Clear "just pressed/released" states at the start of each frame
    m_keysPressed.clear();
    m_keysReleased.clear();
    m_mouseButtonsPressed.clear();
    m_mouseButtonsReleased.clear();
    m_mouseWheel = 0.0f;
}

void Input::update(const SDL_Event& event) {
    switch (event.type) {
        case SDL_EVENT_KEY_DOWN:
            if (!event.key.repeat) {  // Ignore key repeats
                m_keysDown.insert(event.key.key);
                m_keysPressed.insert(event.key.key);
            }
            break;

        case SDL_EVENT_KEY_UP:
            m_keysDown.erase(event.key.key);
            m_keysReleased.insert(event.key.key);
            break;

        case SDL_EVENT_MOUSE_BUTTON_DOWN:
            m_mouseButtonsDown.insert(event.button.button);
            m_mouseButtonsPressed.insert(event.button.button);
            break;

        case SDL_EVENT_MOUSE_BUTTON_UP:
            m_mouseButtonsDown.erase(event.button.button);
            m_mouseButtonsReleased.insert(event.button.button);
            break;

        case SDL_EVENT_MOUSE_MOTION:
            m_mouseX = static_cast<int>(event.motion.x);
            m_mouseY = static_cast<int>(event.motion.y);
            break;

        case SDL_EVENT_MOUSE_WHEEL:
            m_mouseWheel = event.wheel.y;
            break;

        default:
            break;
    }
}

bool Input::isKeyDown(SDL_Keycode key) const {
    return m_keysDown.find(key) != m_keysDown.end();
}

bool Input::isKeyPressed(SDL_Keycode key) const {
    return m_keysPressed.find(key) != m_keysPressed.end();
}

bool Input::isKeyReleased(SDL_Keycode key) const {
    return m_keysReleased.find(key) != m_keysReleased.end();
}

bool Input::isMouseButtonDown(int button) const {
    return m_mouseButtonsDown.find(button) != m_mouseButtonsDown.end();
}

bool Input::isMouseButtonPressed(int button) const {
    return m_mouseButtonsPressed.find(button) != m_mouseButtonsPressed.end();
}

bool Input::isMouseButtonReleased(int button) const {
    return m_mouseButtonsReleased.find(button) != m_mouseButtonsReleased.end();
}

void Input::getMousePosition(int& x, int& y) const {
    x = m_mouseX;
    y = m_mouseY;
}

float Input::getMouseWheel() const {
    return m_mouseWheel;
}

} // namespace Runa
