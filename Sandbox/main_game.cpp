/**
 * main_game.cpp
 * Clean game implementation - blank canvas with FPS display
 */

#include "Core/Application.h"
#include "Core/Log.h"
#include "Graphics/Font.h"
#include "Graphics/SpriteBatch.h"
#include "runapch.h"
#include <memory>
#include <chrono>
#include <string>

class Game : public Runa::Application {
public:
    Game() : Application("Runa2 Game", 1280, 720) {}

protected:
    void onInit() override {
        LOG_INFO("=== Runa2 Game ===");

        // Initialize rendering systems
        m_spriteBatch = std::make_unique<Runa::SpriteBatch>(getRenderer());
        
        // Load font for FPS display
        m_font = std::make_unique<Runa::Font>(getRenderer(),
                                              "Resources/Fonts/Renogare.ttf", 24);
        
        if (!m_font->isValid()) {
            LOG_WARN("Failed to load font, FPS display may not work");
        }

        // Initialize FPS tracking
        m_lastFPSUpdate = std::chrono::steady_clock::now();
        m_displayedFPS = 0;
        m_frameCount = 0;

        LOG_INFO("Game initialized");
    }

    void onUpdate(float deltaTime) override {
        // Count frames for FPS calculation
        m_frameCount++;

        // Update FPS display once per second
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
            now - m_lastFPSUpdate).count();

        if (elapsed >= 1000) { // Update every second
            m_displayedFPS = m_frameCount;
            m_frameCount = 0;
            m_lastFPSUpdate = now;
        }
    }

    void onRender() override {
        // Clear with dark blueish hue
        // RGB: (0.05, 0.08, 0.15) - very dark blue
        getRenderer().clear(0.05f, 0.08f, 0.15f, 1.0f);

        m_spriteBatch->begin();

        // Render FPS counter in white
        if (m_font && m_font->isValid()) {
            std::string fpsText = "FPS: " + std::to_string(m_displayedFPS);
            SDL_Color white = {255, 255, 255, 255};
            auto fpsTexture = m_font->renderText(fpsText, white);
            
            if (fpsTexture) {
                // Position in top-left corner with some padding
                m_spriteBatch->draw(*fpsTexture, 10, 10);
            }
        }

        m_spriteBatch->end();
    }

    void onShutdown() override {
        LOG_INFO("Game shutting down");
    }

private:
    std::unique_ptr<Runa::SpriteBatch> m_spriteBatch;
    std::unique_ptr<Runa::Font> m_font;

    // FPS tracking
    std::chrono::steady_clock::time_point m_lastFPSUpdate;
    int m_displayedFPS = 0;
    int m_frameCount = 0;
};

int main(int argc, char* argv[]) {
    try {
        Runa::Log::init();
        LOG_INFO("Starting Runa2 Game...");

        auto game = std::make_unique<Game>();
        game->run();

        LOG_INFO("Game exited successfully");
        return 0;
    } catch (const std::exception& e) {
        LOG_CRITICAL("Fatal error: {}", e.what());
        return 1;
    }
}
