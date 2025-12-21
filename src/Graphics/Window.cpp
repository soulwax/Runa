// File: src/Graphics/Window.cpp

#include "Window.h"
#include <iostream>
#include <stdexcept>

namespace Runa {

Window::Window(const std::string& title, int width, int height)
    : m_width(width), m_height(height) {

    m_window = SDL_CreateWindow(
        title.c_str(),
        width, height,
        SDL_WINDOW_RESIZABLE
    );

    if (!m_window) {
        throw std::runtime_error(std::string("Failed to create window: ") + SDL_GetError());
    }

    std::cout << "Window created: " << title << " (" << width << "x" << height << ")" << std::endl;
}

Window::~Window() {
    if (m_window) {
        SDL_DestroyWindow(m_window);
        std::cout << "Window destroyed" << std::endl;
    }
}

void Window::processEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_EVENT_QUIT:
                m_shouldClose = true;
                break;

            case SDL_EVENT_WINDOW_RESIZED:
                m_width = event.window.data1;
                m_height = event.window.data2;
                std::cout << "Window resized to " << m_width << "x" << m_height << std::endl;
                break;

            case SDL_EVENT_KEY_DOWN:
                if (event.key.key == SDLK_ESCAPE) {
                    m_shouldClose = true;
                }
                break;
        }
    }
}

} // namespace Runa
