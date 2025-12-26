// File: src/Graphics/Window.cpp

#include "../runapch.h"
#include "Window.h"
#include "Core/Log.h"

namespace Runa
{

    Window::Window(const std::string &title, int width, int height)
        : m_width(width), m_height(height)
    {

        m_window = SDL_CreateWindow(
            title.c_str(),
            width, height,
            SDL_WINDOW_RESIZABLE);

        if (!m_window)
        {
            throw std::runtime_error(std::string("Failed to create window: ") + SDL_GetError());
        }

        LOG_INFO("Window created: {} ({}x{})", title, width, height);
    }

    Window::~Window()
    {
        if (m_window)
        {
            SDL_DestroyWindow(m_window);
            LOG_INFO("Window destroyed");
        }
    }

    void Window::processEvents()
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_EVENT_QUIT:
                m_shouldClose = true;
                break;

            case SDL_EVENT_WINDOW_RESIZED:
                m_width = event.window.data1;
                m_height = event.window.data2;
                LOG_DEBUG("Window resized to {}x{}", m_width, m_height);
                break;

            case SDL_EVENT_KEY_DOWN:
                if (event.key.key == SDLK_ESCAPE)
                {
                    m_shouldClose = true;
                }
                break;
            }
        }
    }

} // namespace Runa
