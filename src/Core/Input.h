

#ifndef RUNA_CORE_INPUT_H
#define RUNA_CORE_INPUT_H

#include "../RunaAPI.h"
#include <SDL3/SDL.h>
#include <unordered_set>

namespace Runa {

class Window;


class RUNA_API Input {
public:
    explicit Input(Window& window);
    ~Input() = default;


    Input(const Input&) = delete;
    Input& operator=(const Input&) = delete;
    Input(Input&&) noexcept = default;
    Input& operator=(Input&&) noexcept = default;


    void update(const SDL_Event& event);
    void beginFrame();


    bool isKeyDown(SDL_Keycode key) const;
    bool isKeyPressed(SDL_Keycode key) const;
    bool isKeyReleased(SDL_Keycode key) const;


    bool isMouseButtonDown(int button) const;
    bool isMouseButtonPressed(int button) const;
    bool isMouseButtonReleased(int button) const;
    void getMousePosition(int& x, int& y) const;
    float getMouseWheel() const;

private:
    Window& m_window;


    std::unordered_set<SDL_Keycode> m_keysDown;
    std::unordered_set<SDL_Keycode> m_keysPressed;
    std::unordered_set<SDL_Keycode> m_keysReleased;


    std::unordered_set<int> m_mouseButtonsDown;
    std::unordered_set<int> m_mouseButtonsPressed;
    std::unordered_set<int> m_mouseButtonsReleased;
    int m_mouseX = 0;
    int m_mouseY = 0;
    float m_mouseWheel = 0.0f;
};

}

#endif
