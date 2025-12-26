// File: src/Core/Application.cpp

#include "../runapch.h"
#include "Application.h"
#include "Log.h"

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
    try {
        onInit();
        LOG_INFO("onInit() completed successfully");
    } catch (const std::exception& e) {
        LOG_CRITICAL("Exception in onInit(): {}", e.what());
        throw;
    } catch (...) {
        LOG_CRITICAL("Unknown exception in onInit()");
        throw;
    }

    LOG_INFO("Starting main loop...");
    mainLoop();
}

void Application::mainLoop() {
    using Clock = std::chrono::high_resolution_clock;
    using Duration = std::chrono::duration<float>;
    using namespace std::chrono_literals;

    auto lastTime = Clock::now();
    float accumulatedTime = 0.0f;
    int frameCount = 0;

    // Target 60 FPS (16.67ms per frame)
    constexpr auto targetFrameTime = std::chrono::duration<float>(1.0f / 60.0f);

    while (m_running && !m_window->shouldClose()) {
        auto frameStart = Clock::now();

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

        // Frame rate limiting: sleep to maintain 60 FPS
        auto frameEnd = Clock::now();
        auto frameDuration = frameEnd - frameStart;
        if (frameDuration < targetFrameTime) {
            auto sleepTime = targetFrameTime - frameDuration;
            std::this_thread::sleep_for(sleepTime);
        }
    }
}

} // namespace Runa
