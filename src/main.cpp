// File: src/main.cpp

#include <iostream>
#include <memory>
#include <fstream>
#include <sstream>
#include <exception>

#include "Core/Application.h"
#include "Core/ResourceManager.h"
#include "Core/Log.h"
#include "Graphics/TileMap.h"
#include "Graphics/SpriteBatch.h"

class GameApp : public Runa::Application {
    std::unique_ptr<Runa::ResourceManager> m_resources;
    std::unique_ptr<Runa::TileMap> m_tileMap;
    std::unique_ptr<Runa::SpriteBatch> m_spriteBatch;

public:
    GameApp() : Application("Runa2 - Tilemap Demo", 1280, 720) {}

protected:
    void onInit() override {
        LOG_INFO("=== Runa2 Tilemap Demo ===");
        LOG_INFO("Loading resources...");

        // Create resource manager
        m_resources = std::make_unique<Runa::ResourceManager>(getRenderer());

        // Load the plains tileset
        try {
            m_resources->loadSpriteSheetFromYAML("Resources/manifests/tilesets.yaml");
            LOG_INFO("Tileset loaded successfully!");
        } catch (const std::exception& e) {
            LOG_ERROR("Failed to load tileset: {}", e.what());
            return;
        }

        // Create tilemap (40x30 tiles, 16 pixels per tile)
        m_tileMap = std::make_unique<Runa::TileMap>(40, 30, 16);

        // Load scene from file
        try {
            std::ifstream file("Resources/scenes/sample_scene.txt");
            if (file) {
                std::stringstream buffer;
                buffer << file.rdbuf();
                m_tileMap->loadFromString(buffer.str());
                LOG_INFO("Scene loaded successfully!");
            } else {
                LOG_INFO("Scene file not found, creating simple pattern...");
                // Create a simple grass pattern as fallback
                for (int y = 0; y < 30; ++y) {
                    for (int x = 0; x < 40; ++x) {
                        m_tileMap->setTile(x, y, (x + y) % 6);
                    }
                }
            }
        } catch (const std::exception& e) {
            LOG_ERROR("Error loading scene: {}", e.what());
        }

        // Create sprite batch
        m_spriteBatch = std::make_unique<Runa::SpriteBatch>(getRenderer());

        LOG_INFO("\n=== Ready ===");
        LOG_INFO("Displaying 40x30 tilemap from plains.png");
        LOG_INFO("Map size: 640x480 pixels (16px tiles)");
        LOG_INFO("Press ESC or close window to exit.");
    }

    void onUpdate(float deltaTime) override {
        // No dynamic updates for this static demo
    }

    void onRender() override {
        // Clear screen with a dark background
        getRenderer().clear(0.05f, 0.05f, 0.1f, 1.0f);

        // Render the tilemap
        if (m_tileMap && m_spriteBatch && m_resources) {
            auto* tileset = m_resources->getSpriteSheet("plains_tileset");
            if (tileset) {
                m_spriteBatch->begin();

                // Center the map on screen (1280x720 screen, 640x480 map)
                int offsetX = (1280 - 640) / 2;
                int offsetY = (720 - 480) / 2;

                m_tileMap->render(*m_spriteBatch, *tileset, "plains_tile", offsetX, offsetY);

                m_spriteBatch->end();
            }
        }
    }

    void onShutdown() override {
        LOG_INFO("Demo shutting down...");
    }
};

int main(int argc, char* argv[]) {
    // Initialize logging before anything else to ensure we can log errors
    // This is a safety measure in case Application constructor fails
    try {
        Runa::Log::init();
    } catch (...) {
        // If logging init fails, we can't log, so use stderr
        std::cerr << "CRITICAL: Failed to initialize logging system!" << std::endl;
    }

    try {
        auto app = std::make_unique<GameApp>();
        app->run();
        return 0;
    } catch (const std::exception& e) {
        // Try to log, but if logging fails, fall back to stderr
        try {
            LOG_CRITICAL("Fatal error: {}", e.what());
        } catch (...) {
            std::cerr << "CRITICAL: Fatal error: " << e.what() << std::endl;
        }
        return 1;
    } catch (...) {
        // Catch-all for non-standard exceptions
        try {
            LOG_CRITICAL("Fatal error: Unknown exception occurred");
        } catch (...) {
            std::cerr << "CRITICAL: Unknown exception occurred" << std::endl;
        }
        return 1;
    }
}
