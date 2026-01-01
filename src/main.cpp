// File: src/main.cpp

#include "Core/Application.h"
#include "Core/Log.h"
#include "Core/ResourceManager.h"
#include "Graphics/Camera.h"
#include "Graphics/Font.h"
#include "Graphics/SpriteBatch.h"
#include "Graphics/TileMap.h"
#include "runapch.h"

// NEW: Include ECS headers
#include "ECS/Components.h"
#include "ECS/Registry.h"
#include "ECS/Systems.h"

class GameApp : public Runa::Application {
public:
  GameApp() : Application("Runa2 - RPG Demo (ECS)", 1280, 720) {}

protected:
  void onInit() override {
    LOG_INFO("=== Runa2 RPG Demo (ECS Version) ===");
    LOG_INFO("Loading resources...");

    // Create core systems
    m_resources = std::make_unique<Runa::ResourceManager>(getRenderer());
    m_spriteBatch = std::make_unique<Runa::SpriteBatch>(getRenderer());
    m_camera = std::make_unique<Runa::Camera>(getWindow());

    // NEW: Create ECS registry
    m_registry = std::make_unique<Runa::ECS::EntityRegistry>();

    // Load tileset
    try {
      m_resources->loadTilesetFromAtlasYAML(
          "Resources/mystic_woods_2.2/sprites/tilesets/plains.yaml",
          "Resources/mystic_woods_2.2/sprites/tilesets/plains.png", "plains");
      LOG_INFO("Plains tileset loaded successfully!");
    } catch (const std::exception &e) {
      LOG_ERROR("Failed to load tileset: {}", e.what());
      throw;
    }

    // Create tilemap
    m_tileMap = std::make_unique<Runa::TileMap>(40, 30, 16);
    m_tileMap->clear(0);

    // Create border walls
    const int WALL_TILE = 12;
    for (int x = 0; x < 40; ++x) {
      m_tileMap->setTile(x, 0, WALL_TILE);
      m_tileMap->setTile(x, 29, WALL_TILE);
    }
    for (int y = 0; y < 30; ++y) {
      m_tileMap->setTile(0, y, WALL_TILE);
      m_tileMap->setTile(39, y, WALL_TILE);
    }

    // Add interior obstacles
    for (int x = 10; x < 15; ++x) {
      m_tileMap->setTile(x, 10, WALL_TILE);
    }
    for (int x = 25; x < 30; ++x) {
      m_tileMap->setTile(x, 20, WALL_TILE);
    }

    m_tileMap->setSolidTile(WALL_TILE, true);
    LOG_INFO("Tilemap created with solid walls");

    // NEW: Create player entity using ECS
    float startX = 20.0f * 16.0f;
    float startY = 15.0f * 16.0f;

    auto *tileset = m_resources->getSpriteSheet("plains");
    if (tileset) {
      m_playerEntity = m_registry->createPlayer(startX, startY, tileset,
                                                "plains_grass_6", 120.0f);

      // Set reddish tint for visibility
      auto &sprite =
          m_registry->getRegistry().get<Runa::ECS::Sprite>(m_playerEntity);
      sprite.tintR = 1.0f;
      sprite.tintG = 0.5f;
      sprite.tintB = 0.5f;

      LOG_INFO("Player entity created at ({}, {})", startX, startY);
    }

    // Load font
    try {
      std::string fontPath = "Resources/Fonts/Renogare.ttf";
      if (!std::filesystem::exists(fontPath)) {
        fontPath = "Resources/Fonts/Renogare.otf";
      }
      m_font = std::make_unique<Runa::Font>(getRenderer(), fontPath, 20);
      LOG_INFO("Font loaded successfully");
    } catch (const std::exception &e) {
      LOG_WARN("Failed to load font: {}", e.what());
    }

    // Set initial camera position
    if (auto *pos = m_registry->getRegistry().try_get<Runa::ECS::Position>(
            m_playerEntity)) {
      m_camera->setPosition(pos->x, pos->y);
    }

    LOG_INFO("=== RPG Demo Ready ===");
    LOG_INFO("Controls:");
    LOG_INFO("  Arrow Keys - Move Player");
    LOG_INFO("  WASD - Move Camera");
    LOG_INFO("  Right/Middle Mouse - Pan Camera");
    LOG_INFO("  Mouse Wheel - Zoom Camera");
    LOG_INFO("  ESC - Quit");
  }

  void onUpdate(float dt) override {
    auto &registry = m_registry->getRegistry();

    // NEW: Run ECS systems
    // 1. Input system - read input and set velocities
    Runa::ECS::Systems::updatePlayerInput(registry, getInput(), dt);

    // 2. Physics system - apply velocities to positions
    Runa::ECS::Systems::updateMovement(registry, dt);

    // 3. Animation system - update sprite frames
    Runa::ECS::Systems::updateAnimation(registry, dt);

    // 4. Collision system - resolve tile collisions
    Runa::ECS::Systems::updateTileCollisions(registry, *m_tileMap, 16);

    // 5. Manual camera controls (WASD, mouse drag, mouse wheel zoom)
    m_camera->handleInput(getInput(), dt, 300.0f);

    // 6. Camera system - follow player (unless manually controlled)
    Runa::ECS::Systems::updateCameraFollow(registry, *m_camera, dt);
    m_camera->update(dt);
  }

  void onRender() override {
    getRenderer().clear(0.1f, 0.15f, 0.2f, 1.0f);

    auto *tileset = m_resources->getSpriteSheet("plains");
    if (!tileset)
      return;

    m_spriteBatch->begin();

    // Render tilemap
    auto bounds = m_camera->getWorldBounds();
    int tileSize = m_tileMap->getTileSize();

    int startTileX = std::max(0, static_cast<int>(bounds.left / tileSize) - 1);
    int endTileX = std::min(m_tileMap->getWidth() - 1,
                            static_cast<int>(bounds.right / tileSize) + 1);
    int startTileY = std::max(0, static_cast<int>(bounds.top / tileSize) - 1);
    int endTileY = std::min(m_tileMap->getHeight() - 1,
                            static_cast<int>(bounds.bottom / tileSize) + 1);

    auto tileNames = tileset->getSpriteNames();
    for (int y = startTileY; y <= endTileY; ++y) {
      for (int x = startTileX; x <= endTileX; ++x) {
        int tileIndex = m_tileMap->getTile(x, y);
        if (tileIndex < 0 || tileIndex >= static_cast<int>(tileNames.size())) {
          continue;
        }

        const std::string &tileName = tileNames[tileIndex];
        const Runa::Sprite *sprite = tileset->getSprite(tileName);

        if (sprite && !sprite->frames.empty()) {
          float worldX = x * tileSize;
          float worldY = y * tileSize;
          int screenX, screenY;
          m_camera->worldToScreen(worldX, worldY, screenX, screenY);
          m_spriteBatch->draw(tileset->getTexture(), screenX, screenY,
                              sprite->frames[0]);
        }
      }
    }

    // NEW: Render all sprites using ECS system
    Runa::ECS::Systems::renderSprites(m_registry->getRegistry(), *m_spriteBatch,
                                      *m_camera);

    // Render FPS counter
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
    LOG_INFO("Total entities created: {}", m_registry->getEntityCount());
  }

private:
  std::unique_ptr<Runa::ResourceManager> m_resources;
  std::unique_ptr<Runa::SpriteBatch> m_spriteBatch;
  std::unique_ptr<Runa::Camera> m_camera;
  std::unique_ptr<Runa::TileMap> m_tileMap;
  std::unique_ptr<Runa::Font> m_font;
  std::unique_ptr<Runa::Texture> m_fpsTexture;
  int m_lastFPS = -1;

  // NEW: ECS registry and player entity
  std::unique_ptr<Runa::ECS::EntityRegistry> m_registry;
  entt::entity m_playerEntity;
};

int main(int argc, char *argv[]) {
  try {
    Runa::Log::init();
    auto app = std::make_unique<GameApp>();
    app->run();
    return 0;
  } catch (const std::exception &e) {
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
