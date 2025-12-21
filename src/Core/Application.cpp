// File: src/Application.cpp

#include "Application.h"
#include <iostream>
#include <chrono>

namespace Runa {

Application::Application(const std::string& title, int width, int height) {
    // Initialize SDL
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS)) {
        throw std::runtime_error(std::string("Failed to initialize SDL: ") + SDL_GetError());
    }

    std::cout << "SDL3 initialized successfully" << std::endl;

    // Create window and renderer
    m_window = std::make_unique<Window>(title, width, height);
    m_renderer = std::make_unique<Renderer>(*m_window);
}

Application::~Application() {
    onShutdown();

    m_renderer.reset();
    m_window.reset();

    SDL_Quit();
    std::cout << "Application shut down" << std::endl;
}

void Application::run() {
    m_running = true;

    // Initialize game-specific resources
    onInit();

    std::cout << "Starting main loop..." << std::endl;
    mainLoop();
}

void Application::mainLoop() {
    using Clock = std::chrono::high_resolution_clock;
    using Duration = std::chrono::duration<float>;

    auto lastTime = Clock::now();
    float accumulatedTime = 0.0f;
    int frameCount = 0;

    while (m_running && !m_window->shouldClose()) {
        // Calculate delta time
        auto currentTime = Clock::now();
        float deltaTime = Duration(currentTime - lastTime).count();
        lastTime = currentTime;

        // FPS counter
        accumulatedTime += deltaTime;
        frameCount++;
        if (accumulatedTime >= 1.0f) {
            std::cout << "FPS: " << frameCount << std::endl;
            frameCount = 0;
            accumulatedTime = 0.0f;
        }

        // Process events
        m_window->processEvents();

        // Update game logic
        onUpdate(deltaTime);

        // Render
        m_renderer->beginFrame();
        onRender();
        m_renderer->endFrame();
    }
}

} // namespace Runa
