// File: src/grass_test.cpp
// Simple grass tile rendering test with Vulkan2D

#include "Core/Application.h"
#include "Core/Log.h"
#include "Graphics/SpriteBatch.h"
#include "Graphics/Texture.h"
#include <memory>
#include <string>

class GrassTest : public Runa::Application {
public:
  GrassTest() : Application("Grass Tiles - Vulkan2D", 1280, 720) {}

protected:
  void onInit() override {
    LOG_INFO("=== Grass Tile Test ===");

    m_spriteBatch = std::make_unique<Runa::SpriteBatch>(getRenderer());

    try {
      // Load grass texture
      m_grassTexture = std::make_unique<Runa::Texture>(
          getRenderer(), "Resources/SpiteSheets/decor-grass.png");
      LOG_INFO("Grass texture loaded successfully! ({}x{})",
               m_grassTexture->getWidth(), m_grassTexture->getHeight());

      // Load dirt path texture
      m_dirtTexture = std::make_unique<Runa::Texture>(
          getRenderer(), "Resources/SpiteSheets/dirt-grass.png");
      LOG_INFO("Dirt texture loaded successfully! ({}x{})",
               m_dirtTexture->getWidth(), m_dirtTexture->getHeight());
    } catch (const std::exception &e) {
      LOG_ERROR("Failed to load textures: {}", e.what());
    }
  }

  void onUpdate(float dt) override {}

  void onRender() override {
    // Note: beginFrame() and endFrame() are called by Application::mainLoop()
    getRenderer().clear(0.05f, 0.1f, 0.05f, 1.0f);

    if (!m_grassTexture || !m_dirtTexture) {
      LOG_ERROR("Textures not loaded!");
      return;
    }

    m_spriteBatch->begin();

    // Draw grass and dirt tiles across the screen
    // decor-grass.png is a 64x80 sprite sheet (4x5 tiles of 16x16 each)
    // dirt-grass.png is a 64x64 sprite sheet (4x4 tiles of 16x16 each)
    const int tileSize = 16;
    const int tilesX = 1280 / tileSize; // 80 tiles wide
    const int tilesY = 720 / tileSize;  // 45 tiles tall

    // Define dirt paths (vertical and horizontal)
    const int pathX = tilesX / 2; // Vertical path at center
    const int pathY = tilesY / 2; // Horizontal path at center

    for (int y = 0; y < tilesY; ++y) {
      for (int x = 0; x < tilesX; ++x) {
        bool isPath = (x == pathX || y == pathY);

        if (isPath) {
          // Draw dirt path tile from dirt-grass.png
          // Use tile at (16, 0) - horizontal dirt path
          m_spriteBatch->draw(*m_dirtTexture, x * tileSize, y * tileSize, 16, 0,
                              tileSize, tileSize);
        } else {
          // Draw grass tile from decor-grass.png
          m_spriteBatch->draw(*m_grassTexture, x * tileSize, y * tileSize, 0, 0,
                              tileSize, tileSize);
        }
      }
    }

    m_spriteBatch->end();

    if (m_frameCount == 0) {
      const int totalTiles = tilesX * tilesY;
      const std::string message =
          "First frame rendered: " + std::to_string(totalTiles) +
          " tiles with dirt paths (" + std::to_string(tilesX) + "x" +
          std::to_string(tilesY) + ")";
      LOG_INFO(message.c_str());
    }
    m_frameCount++;
  }

  void onShutdown() override { LOG_INFO("Shutting down grass test"); }

private:
  std::unique_ptr<Runa::SpriteBatch> m_spriteBatch;
  std::unique_ptr<Runa::Texture> m_grassTexture;
  std::unique_ptr<Runa::Texture> m_dirtTexture;
  int m_frameCount = 0;
};

int main(int argc, char *argv[]) {
  try {
    Runa::Log::init();
    auto app = std::make_unique<GrassTest>();
    app->run();
    return 0;
  } catch (const std::exception &e) {
    try {
      LOG_CRITICAL("Fatal error: {}", e.what());
    } catch (...) {
      std::cerr << "Fatal error: " << e.what() << std::endl;
    }
    return 1;
  }
}
