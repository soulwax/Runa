// File: src/Graphics/Window.h

#pragma once

#include "../RunaAPI.h"
#include <SDL3/SDL.h>
#include <string>

namespace Runa {

class RUNA_API Window {
public:
    Window(const std::string& title, int width, int height);
    ~Window();

    // Disable copying
    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

    // Enable moving
    Window(Window&&) noexcept = default;
    Window& operator=(Window&&) noexcept = default;

    SDL_Window* getHandle() const { return m_window; }
    int getWidth() const { return m_width; }
    int getHeight() const { return m_height; }
    bool shouldClose() const { return m_shouldClose; }

    void processEvents();
    void setShouldClose(bool shouldClose) { m_shouldClose = shouldClose; }

private:
    SDL_Window* m_window = nullptr;
    int m_width;
    int m_height;
    bool m_shouldClose = false;
};

} // namespace Runa
