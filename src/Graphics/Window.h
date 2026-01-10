// File: src/Graphics/Window.h

#ifndef RUNA_GRAPHICS_WINDOW_H
#define RUNA_GRAPHICS_WINDOW_H

#include "../RunaAPI.h"
#include <SDL3/SDL.h>
#include <string>

namespace Runa {

class Input;

class RUNA_API Window {
public:
    Window(const std::string& title, int width, int height);
    ~Window();


    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;
    Window(Window&&) = delete;
    Window& operator=(Window&&) = delete;

    SDL_Window* getHandle() const { return m_window; }
    int getWidth() const { return m_width; }
    int getHeight() const { return m_height; }
    bool shouldClose() const { return m_shouldClose; }

    void processEvents();
    void setShouldClose(bool shouldClose) { m_shouldClose = shouldClose; }


    void setInput(Input* input) { m_input = input; }

private:
    SDL_Window* m_window = nullptr;
    int m_width;
    int m_height;
    bool m_shouldClose = false;
    Input* m_input = nullptr;
};

}

#endif
