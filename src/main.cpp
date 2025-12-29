// File: src/main.cpp

#include "runapch.h"
#include "Core/Application.h"
#include "Core/ResourceManager.h"
#include "Core/Entity.h"
#include "Core/PlayerController.h"
#include "Core/Collision.h"
#include "Core/Log.h"
#include "Graphics/SpriteBatch.h"
#include "Graphics/Camera.h"
#include "Graphics/TileMap.h"
#include "Graphics/Font.h"

// Simple player entity that inherits from Entity
class Player : public Runa::Entity {
public:
    Player(float x, float y) : Entity(x, y, 14, 14) {}  // Slightly smaller than tile for better collision feel

    void update(float dt) override {
        // Apply velocity to position (velocity is set by PlayerController)
        move(m_velocityX * dt, m_velocityY * dt);

        // Update animation
        Entity::update(dt);
    }
};

class GameApp : public Runa::Application {
public:
    GameApp() : Application("Runa2 - RPG Demo", 1280, 720) {}

protected:
    void onInit() override {
        LOG_INFO("=== Runa2 RPG Demo ===");
        LOG_INFO("Loading resources...");

        // Create core systems
        m_resources = std::make_unique<Runa::ResourceManager>(getRenderer());
        m_spriteBatch = std::make_unique<Runa::SpriteBatch>(getRenderer());
        m_camera = std::make_unique<Runa::Camera>(getWindow());
        m_collisionManager = std::make_unique<Runa::CollisionManager>();

        // Load tileset
        try {
            m_resources->loadTilesetFromAtlasYAML(
                "Resources/mystic_woods_2.2/sprites/tilesets/plains.yaml",
                "Resources/mystic_woods_2.2/sprites/tilesets/plains.png",
                "plains"
            );
            LOG_INFO("Plains tileset loaded successfully!");
        } catch (const std::exception& e) {
            LOG_ERROR("Failed to load tileset: {}", e.what());
            throw;
        }

        // Create tilemap (40x30 tiles, 16px each = 640x480 world)
        m_tileMap = std::make_unique<Runa::TileMap>(40, 30, 16);

        // Fill with grass (tile 0 - plains_grass_0)
        m_tileMap->clear(0);

        // Create border walls using tile 12
        const int WALL_TILE = 12;
        for (int x = 0; x < 40; ++x) {
            m_tileMap->setTile(x, 0, WALL_TILE);   // Top wall
            m_tileMap->setTile(x, 29, WALL_TILE);  // Bottom wall
        }
        for (int y = 0; y < 30; ++y) {
            m_tileMap->setTile(0, y, WALL_TILE);   // Left wall
            m_tileMap->setTile(39, y, WALL_TILE);  // Right wall
        }

        // Add some interior obstacles
        for (int x = 10; x < 15; ++x) {
            m_tileMap->setTile(x, 10, WALL_TILE);
        }
        for (int x = 25; x < 30; ++x) {
            m_tileMap->setTile(x, 20, WALL_TILE);
        }

        // Mark wall tiles as solid for collision
        m_tileMap->setSolidTile(WALL_TILE, true);

        LOG_INFO("Tilemap created with solid walls");

        // Create player at center of map (in world coordinates)
        float startX = 20.0f * 16.0f;  // Center tile X
        float startY = 15.0f * 16.0f;  // Center tile Y
        m_player = std::make_unique<Player>(startX, startY);

        // Set player sprite (use a grass tile for now - you can replace with character sprite later)
        auto* tileset = m_resources->getSpriteSheet("plains");
        if (tileset) {
            // Use tile 6 (a flower tile) as the player sprite for visibility
            m_player->setSprite(tileset, "plains_grass_6");
        }

        // Create player controller
        m_playerController = std::make_unique<Runa::PlayerController>(*m_player, getInput());
        m_playerController->setSpeed(120.0f);  // 120 pixels per second

        LOG_INFO("Player created at ({}, {})", startX, startY);

        // Load font for FPS and instructions
        try {
            std::string fontPath = "Resources/Fonts/Renogare.ttf";
            if (!std::filesystem::exists(fontPath)) {
                fontPath = "Resources/Fonts/Renogare.otf";
            }
            m_font = std::make_unique<Runa::Font>(getRenderer(), fontPath, 20);
            LOG_INFO("Font loaded successfully");
        } catch (const std::exception& e) {
            LOG_WARN("Failed to load font: {}", e.what());
        }

        // Set initial camera position to player
        m_camera->setPosition(m_player->getX(), m_player->getY());

        LOG_INFO("=== RPG Demo Ready ===");
        LOG_INFO("Controls:");
        LOG_INFO("  WASD or Arrow Keys - Move");
        LOG_INFO("  ESC - Quit");
    }

    void onUpdate(float dt) override {
        // Update player controller (read input, set velocity)
        m_playerController->update(dt);

        // Update player (apply velocity to position)
        m_player->update(dt);

        // Check collisions with tilemap and resolve
        m_collisionManager->checkTileCollisions(*m_player, *m_tileMap, 16);

        // Camera follows player smoothly
        m_camera->follow(*m_player, 0.15f);  // Smooth follow with lerp factor
        m_camera->update(dt);
    }

    void onRender() override {
        // Clear screen with dark blue background
        getRenderer().clear(0.1f, 0.15f, 0.2f, 1.0f);

        auto* tileset = m_resources->getSpriteSheet("plains");
        if (!tileset) return;

        m_spriteBatch->begin();

        // Get camera bounds for culling
        auto bounds = m_camera->getWorldBounds();
        int tileSize = m_tileMap->getTileSize();

        // Calculate which tiles are visible
        int startTileX = std::max(0, static_cast<int>(bounds.left / tileSize) - 1);
        int endTileX = std::min(m_tileMap->getWidth() - 1, static_cast<int>(bounds.right / tileSize) + 1);
        int startTileY = std::max(0, static_cast<int>(bounds.top / tileSize) - 1);
        int endTileY = std::min(m_tileMap->getHeight() - 1, static_cast<int>(bounds.bottom / tileSize) + 1);

        // Render tilemap with camera transformation
        auto tileNames = tileset->getSpriteNames();
        for (int y = startTileY; y <= endTileY; ++y) {
            for (int x = startTileX; x <= endTileX; ++x) {
                int tileIndex = m_tileMap->getTile(x, y);
                if (tileIndex < 0 || tileIndex >= static_cast<int>(tileNames.size())) {
                    continue;
                }

                // Get tile sprite
                const std::string& tileName = tileNames[tileIndex];
                const Runa::Sprite* sprite = tileset->getSprite(tileName);

                if (sprite && !sprite->frames.empty()) {
                    // World position
                    float worldX = x * tileSize;
                    float worldY = y * tileSize;

                    // Transform to screen coordinates
                    int screenX, screenY;
                    m_camera->worldToScreen(worldX, worldY, screenX, screenY);

                    // Draw tile
                    m_spriteBatch->draw(tileset->getTexture(), screenX, screenY, sprite->frames[0]);
                }
            }
        }

        // Render player with camera transformation
        int playerScreenX, playerScreenY;
        m_camera->worldToScreen(m_player->getX(), m_player->getY(), playerScreenX, playerScreenY);

        const Runa::Sprite* playerSprite = tileset->getSprite("plains_grass_6");
        if (playerSprite && !playerSprite->frames.empty()) {
            m_spriteBatch->draw(tileset->getTexture(), playerScreenX, playerScreenY,
                              playerSprite->frames[0],
                              1.0f, 0.5f, 0.5f, 1.0f);  // Reddish tint to make player visible
        }

        // Render FPS counter (always in screen space, not world space)
        if (m_font && m_font->isValid()) {
            int fps = getFPS();
            if (fps > 0 && fps != m_lastFPS) {
                std::stringstream fpsText;
                fpsText << "FPS: " << fps;
                m_fpsTexture = m_font->renderText(fpsText.str(), {255, 255, 255, 255});
                m_lastFPS = fps;
            }

            if (m_fpsTexture && m_fpsTexture->isValid()) {
                m_spriteBatch->draw(*m_fpsTexture, 10, 10);
            }
        }

        m_spriteBatch->end();
    }

    void onShutdown() override {
        LOG_INFO("RPG Demo shutting down...");
    }

private:
    std::unique_ptr<Runa::ResourceManager> m_resources;
    std::unique_ptr<Runa::SpriteBatch> m_spriteBatch;
    std::unique_ptr<Runa::Camera> m_camera;
    std::unique_ptr<Runa::TileMap> m_tileMap;
    std::unique_ptr<Player> m_player;
    std::unique_ptr<Runa::PlayerController> m_playerController;
    std::unique_ptr<Runa::CollisionManager> m_collisionManager;
    std::unique_ptr<Runa::Font> m_font;
    std::unique_ptr<Runa::Texture> m_fpsTexture;
    int m_lastFPS = -1;
};

int main(int argc, char* argv[]) {
    try {
        Runa::Log::init();
        auto app = std::make_unique<GameApp>();
        app->run();
        return 0;
    } catch (const std::exception& e) {
        try {
            LOG_CRITICAL("Fatal error: {}", e.what());
        } catch (...) {
            std::cerr << "CRITICAL: Fatal error: " << e.what() << std::endl;
        }
        return 1;
    } catch (...) {
        try {
            LOG_CRITICAL("Fatal error: Unknown exception occurred");
        } catch (...) {
            std::cerr << "CRITICAL: Unknown exception occurred" << std::endl;
        }
        return 1;
    }
}
