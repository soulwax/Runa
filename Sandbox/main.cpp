


#include "Core/Application.h"
#include "Core/Log.h"
#include "Graphics/SpriteBatch.h"
#include "Graphics/Texture.h"
#include "runapch.h"


#include "ECS/Components.h"
#include "ECS/Registry.h"
#include "ECS/Systems.h"

class GameApp : public Runa::Application {
public:
  GameApp() : Application("Runa2 - ECS Demo", 1280, 720) {}

protected:
  void onInit() override {
    LOG_INFO("=== Runa2 ECS Demo ===");


    m_spriteBatch = std::make_unique<Runa::SpriteBatch>(getRenderer());


    m_registry = std::make_unique<Runa::ECS::EntityRegistry>();


    try {
      m_grassTexture = std::make_unique<Runa::Texture>(
          getRenderer(), "Resources/SpiteSheets/decor-grass.png");
      m_dirtTexture = std::make_unique<Runa::Texture>(
          getRenderer(), "Resources/SpiteSheets/dirt-grass.png");
      LOG_INFO("Textures loaded: grass ({}x{}), dirt ({}x{})",
               m_grassTexture->getWidth(), m_grassTexture->getHeight(),
               m_dirtTexture->getWidth(), m_dirtTexture->getHeight());
    } catch (const std::exception &e) {
      LOG_ERROR("Failed to load textures: {}", e.what());
      throw;
    }


    float playerX = 640.0f;
    float playerY = 360.0f;

    auto &reg = m_registry->getRegistry();
    m_playerEntity = reg.create();

    reg.emplace<Runa::ECS::Position>(m_playerEntity, playerX, playerY);
    reg.emplace<Runa::ECS::Velocity>(m_playerEntity, 0.0f, 0.0f);
    reg.emplace<Runa::ECS::Size>(m_playerEntity, 16.0f, 16.0f);
    reg.emplace<Runa::ECS::PlayerInput>(m_playerEntity,
                                        200.0f);
    reg.emplace<Runa::ECS::Player>(m_playerEntity);

    LOG_INFO("Player entity created at ({}, {})", playerX, playerY);

    LOG_INFO("=== Controls ===");
    LOG_INFO("  Arrow Keys / WASD - Move Player");
    LOG_INFO("  ESC - Quit");
  }

  void onUpdate(float dt) override {
    auto &reg = m_registry->getRegistry();



    Runa::ECS::Systems::updatePlayerInput(reg, getInput(), dt);


    Runa::ECS::Systems::updateMovement(reg, dt);
  }

  void onRender() override {
    getRenderer().clear(0.05f, 0.1f, 0.05f, 1.0f);

    if (!m_grassTexture || !m_dirtTexture) {
      return;
    }

    m_spriteBatch->begin();


    const int tileSize = 16;
    const int tilesX = 1280 / tileSize;
    const int tilesY = 720 / tileSize;

    for (int y = 0; y < tilesY; ++y) {
      for (int x = 0; x < tilesX; ++x) {
        m_spriteBatch->draw(*m_grassTexture, x * tileSize, y * tileSize, 0, 0,
                            tileSize, tileSize);
      }
    }


    auto &reg = m_registry->getRegistry();
    auto view =
        reg.view<Runa::ECS::Position, Runa::ECS::Size, Runa::ECS::Player>();

    for (auto entity : view) {
      auto &pos = view.get<Runa::ECS::Position>(entity);
      auto &size = view.get<Runa::ECS::Size>(entity);


      m_spriteBatch->draw(
          *m_dirtTexture, static_cast<int>(pos.x), static_cast<int>(pos.y), 16,
          0,
          static_cast<int>(size.width), static_cast<int>(size.height), 1.0f,
          0.5f, 0.5f, 1.0f);
    }

    m_spriteBatch->end();


    if (m_frameCount % 60 == 0) {
      LOG_DEBUG("FPS: {}", getFPS());
    }
    m_frameCount++;
  }

  void onShutdown() override {
    LOG_INFO("ECS Demo shutting down...");
    LOG_INFO("Total entities: {}", m_registry->getEntityCount());
  }

private:
  std::unique_ptr<Runa::SpriteBatch> m_spriteBatch;
  std::unique_ptr<Runa::Texture> m_grassTexture;
  std::unique_ptr<Runa::Texture> m_dirtTexture;
  std::unique_ptr<Runa::ECS::EntityRegistry> m_registry;
  entt::entity m_playerEntity;
  int m_frameCount = 0;
};

int main(int /*argc*/, char * /*argv*/[]) {
  try {
    Runa::Log::init();
    auto app = std::make_unique<GameApp>();
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
