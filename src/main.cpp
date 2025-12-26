// File: src/main.cpp

#include "runapch.h"
#include <random>
#include "Core/Application.h"
#include "Core/ResourceManager.h"
#include "Core/Log.h"
#include "Graphics/TileMap.h"
#include "Graphics/SpriteBatch.h"
#include "Graphics/PostProcess.h"
#include "Graphics/Font.h"

namespace {
    // Helper function to find project root and resolve resource paths
    std::string resolveResourcePath(const std::string& resourcePath) {
        // Simple approach: just use relative path from current directory
        // The working directory should be set to the project root when running
        return resourcePath;
    }

    // Generate a serene grass plains scene
    void generateSerenePlainsScene(Runa::TileMap& tileMap) {
        const int width = tileMap.getWidth();
        const int height = tileMap.getHeight();
        
        // Simple seeded random number generator for consistent results
        std::mt19937 rng(42); // Fixed seed for consistent scene
        
        // Base grass layer - use varied grass tiles (0-5 for basic, 6-11 for flowers)
        std::uniform_int_distribution<int> basicGrass(0, 5);
        std::uniform_int_distribution<int> flowerGrass(6, 11);
        std::uniform_int_distribution<int> grassChoice(0, 4); // 80% basic, 20% flowers
        
        // Fill with natural grass variation
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                // Mostly basic grass with occasional flower patches
                if (grassChoice(rng) < 4) {
                    tileMap.setTile(x, y, basicGrass(rng));
                } else {
                    tileMap.setTile(x, y, flowerGrass(rng));
                }
            }
        }
        
        // Create a winding path through the scene
        // Path tiles: 36-47 (12 path tiles available)
        std::uniform_int_distribution<int> pathTile(36, 42); // Use first 7 path tiles
        
        // Create a gentle curved path from left to right
        int pathX = width / 5;
        int pathY = height / 3;
        int targetX = width * 4 / 5;
        int targetY = height * 2 / 3;
        
        // Draw the path with smooth curves
        std::uniform_int_distribution<int> pathVariation(-1, 1);
        std::uniform_int_distribution<int> curveChance(0, 10);
        
        // Draw path from start to end
        while (pathY < targetY || pathX < targetX) {
            // Set path tile
            tileMap.setTile(pathX, pathY, pathTile(rng));
            
            // Move towards target with gentle curves
            if (pathX < targetX) {
                pathX++;
                // Occasionally curve up or down
                if (curveChance(rng) < 2 && pathY > 2 && pathY < height - 3) {
                    pathY += pathVariation(rng);
                }
            }
            
            if (pathY < targetY) {
                pathY++;
                // Occasionally curve left or right
                if (curveChance(rng) < 2 && pathX > 2 && pathX < width - 3) {
                    pathX += pathVariation(rng);
                }
            }
            
            // Keep path in bounds
            pathX = std::max(2, std::min(width - 3, pathX));
            pathY = std::max(2, std::min(height - 3, pathY));
        }
        
        // Add decorative elements (tiles 60-71) scattered naturally
        std::uniform_int_distribution<int> decorTile(60, 71);
        std::uniform_int_distribution<int> decorChance(0, 100);
        
        // Place decorative elements (rocks, flowers, etc.) - about 3% of tiles
        // Avoid placing on path
        for (int y = 3; y < height - 3; ++y) {
            for (int x = 3; x < width - 3; ++x) {
                // Skip path tiles
                int currentTile = tileMap.getTile(x, y);
                if (currentTile >= 36 && currentTile <= 47) {
                    continue;
                }
                
                // 3% chance to place a decorative element
                if (decorChance(rng) < 3) {
                    tileMap.setTile(x, y, decorTile(rng));
                }
            }
        }
        
        // Add flower patches (grass with flowers) in natural clusters
        std::uniform_int_distribution<int> clusterX(8, width - 9);
        std::uniform_int_distribution<int> clusterY(8, height - 9);
        
        // Create 5-7 flower clusters of varying sizes
        int numClusters = 6;
        for (int i = 0; i < numClusters; ++i) {
            int centerX = clusterX(rng);
            int centerY = clusterY(rng);
            
            // Random cluster size (2x2 to 4x4)
            std::uniform_int_distribution<int> clusterSize(2, 4);
            int size = clusterSize(rng);
            
            // Create a cluster of flower grass
            for (int dy = -size/2; dy <= size/2; ++dy) {
                for (int dx = -size/2; dx <= size/2; ++dx) {
                    int x = centerX + dx;
                    int y = centerY + dy;
                    if (x >= 0 && x < width && y >= 0 && y < height) {
                        int currentTile = tileMap.getTile(x, y);
                        // Don't overwrite path or decorations
                        if ((currentTile < 36 || currentTile > 47) && 
                            (currentTile < 60 || currentTile > 71)) {
                            tileMap.setTile(x, y, flowerGrass(rng));
                        }
                    }
                }
            }
        }
        
        // Add some border grass variations for natural edges
        std::uniform_int_distribution<int> borderTile(12, 35); // Border and corner tiles
        for (int x = 0; x < width; ++x) {
            // Top border
            if (tileMap.getTile(x, 0) < 36) {
                tileMap.setTile(x, 0, basicGrass(rng));
            }
            // Bottom border
            if (tileMap.getTile(x, height - 1) < 36) {
                tileMap.setTile(x, height - 1, basicGrass(rng));
            }
        }
        for (int y = 0; y < height; ++y) {
            // Left border
            if (tileMap.getTile(0, y) < 36) {
                tileMap.setTile(0, y, basicGrass(rng));
            }
            // Right border
            if (tileMap.getTile(width - 1, y) < 36) {
                tileMap.setTile(width - 1, y, basicGrass(rng));
            }
        }
    }
}

class GameApp : public Runa::Application {
    std::unique_ptr<Runa::ResourceManager> m_resources;
    std::unique_ptr<Runa::TileMap> m_tileMap;
    std::unique_ptr<Runa::SpriteBatch> m_spriteBatch;
    std::unique_ptr<Runa::PostProcess> m_postProcess;
    std::unique_ptr<Runa::Font> m_font;
    std::unique_ptr<Runa::Texture> m_fpsTexture;
    std::chrono::steady_clock::time_point m_startTime;

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

        // Generate serene grass plains scene
        LOG_INFO("Generating serene grass plains scene...");
        generateSerenePlainsScene(*m_tileMap);
        LOG_INFO("Scene generated successfully!");
        
        LOG_DEBUG("Scene loading completed, creating sprite batch...");

        // Create sprite batch
        try {
            LOG_DEBUG("Creating SpriteBatch...");
            m_spriteBatch = std::make_unique<Runa::SpriteBatch>(getRenderer());
            LOG_DEBUG("SpriteBatch created successfully");
        } catch (const std::exception& e) {
            LOG_ERROR("Failed to create SpriteBatch: {}", e.what());
            throw;
        }
        
        // Create post-process effect
        try {
            LOG_DEBUG("Creating PostProcess...");
            m_postProcess = std::make_unique<Runa::PostProcess>(getRenderer());
            LOG_DEBUG("PostProcess created successfully");
        } catch (const std::exception& e) {
            LOG_ERROR("Failed to create PostProcess: {}", e.what());
            throw;
        }
        
        // Load font for FPS display
        try {
            std::string fontPath = resolveResourcePath("Resources/Fonts/Renogare.otf");
            m_font = std::make_unique<Runa::Font>(getRenderer(), fontPath, 24);
            if (m_font->isValid()) {
                m_font = std::make_unique<Runa::Font>(getRenderer(), fontPath, 24);
                if (!m_font->isValid()) {
                    LOG_WARN("Failed to load font for FPS display");
                } else {
                    LOG_INFO("Font loaded successfully");
                }
            }
        } catch (const std::exception& e) {
            LOG_WARN("Failed to create font: {}", e.what());
        }
        
        // Initialize start time for shader animation
        m_startTime = std::chrono::steady_clock::now();

        LOG_INFO("=== Ready ===");
        LOG_INFO("Displaying serene grass plains scene");
        LOG_INFO("Map size: 640x480 pixels (40x30 tiles at 16px each)");
        LOG_INFO("Features: Varied grass, winding path, decorative elements, flower clusters");
        LOG_INFO("Press ESC or close window to exit.");
    }

    void onUpdate(float deltaTime) override {
        // TODO: No dynamic updates for static DEMO
    }

    void onRender() override {
        // Calculate time for shader animation
        auto currentTime = std::chrono::steady_clock::now();
        float time = std::chrono::duration<float>(currentTime - m_startTime).count();
        
        // Clear screen with a serene sky blue background
        getRenderer().clear(0.53f, 0.81f, 0.92f, 1.0f); // Light sky blue

        // Render the tilemap
        if (m_tileMap && m_spriteBatch && m_resources) {
            auto* tileset = m_resources->getSpriteSheet("plains_tileset");
            if (tileset) {
                m_spriteBatch->begin();

                // Center the map on screen (1280x720 screen, 640x480 map)
                int offsetX = (1280 - 640) >> 1;
                int offsetY = (720 - 480) >> 1;

                m_tileMap->render(*m_spriteBatch, *tileset, "plains_tile", offsetX, offsetY);

                m_spriteBatch->end();
            }
        }
        
        // Render FPS in upper left corner
        if (m_font && m_font->isValid() && m_spriteBatch) {
            int fps = getFPS();
            if (fps > 0) {
                std::stringstream fpsText;
                fpsText << "FPS: " << fps;
                
                // Render text to texture
                m_fpsTexture = m_font->renderText(fpsText.str(), {255, 255, 255, 255});
                
                if (m_fpsTexture && m_fpsTexture->isValid()) {
                    // Draw FPS text in upper left corner
                    m_spriteBatch->begin();
                    m_spriteBatch->draw(*m_fpsTexture, 10, 10);
                    m_spriteBatch->end();
                }
            }
        }
        
        // DISABLED: Post-process effect causes crash - swapchain texture needs SAMPLER usage flag
        // TODO: Fix post-processing by rendering to intermediate texture first
        /*
        if (m_postProcess) {
            // Get the current swapchain and apply effect
            SDL_GPUCommandBuffer* cmdBuffer = getRenderer().acquireCommandBuffer();
            if (cmdBuffer) {
                SDL_GPUTexture* swapchainTexture = nullptr;
                if (SDL_AcquireGPUSwapchainTexture(cmdBuffer, getWindow().getHandle(), &swapchainTexture, nullptr, nullptr) && swapchainTexture) {
                    // Apply psychedelic effect (blit with warping)
                    m_postProcess->applyPsychedelic(cmdBuffer, swapchainTexture, swapchainTexture, time);
                    SDL_SubmitGPUCommandBuffer(cmdBuffer);
                } else {
                    SDL_CancelGPUCommandBuffer(cmdBuffer);
                }
            }
        }
        */
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
