// File: src/Core/Application.cpp

#include "Application.h"
#include "Log.h"
#include <SDL3_ttf/SDL_ttf.h>
#include <chrono>

namespace Runa {

Application::Application(const std::string& title, int width, int height) {
    // Note: Logging should be initialized in main() before creating Application
    // This ensures we can log errors during Application construction
    
    // Initialize SDL
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS)) {
        throw std::runtime_error(std::string("Failed to initialize SDL: ") + SDL_GetError());
    }

    LOG_INFO("SDL3 initialized successfully");

    // Initialize SDL_ttf
    if (!TTF_Init()) {
        LOG_WARN("Failed to initialize SDL_ttf: {}", SDL_GetError());
        // Don't throw - font rendering is optional
    } else {
        LOG_INFO("SDL3_ttf initialized successfully");
    }

    // Create window and renderer
    m_window = std::make_unique<Window>(title, width, height);
    m_renderer = std::make_unique<Renderer>(*m_window);
}

Application::~Application() {
    onShutdown();

    m_renderer.reset();
    m_window.reset();

    // Quit SDL_ttf if it was initialized
    if (TTF_WasInit()) {
        TTF_Quit();
    }

    SDL_Quit();
    LOG_INFO("Application shut down");
    Log::shutdown();
}

void Application::run() {
    m_running = true;

    // Initialize game-specific resources
    onInit();

    LOG_INFO("Starting main loop...");
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
            m_currentFPS = frameCount;
            LOG_DEBUG("FPS: {}", m_currentFPS);
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
