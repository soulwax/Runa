/**
 * main_rpg.cpp
 * Runa RPG - A top-down action RPG - with a vulkan graphics backend and engine
 * with ECS and SDL3.
 */

#include "Core/Application.h"
#include "Core/InputManager.h"
#include "Core/Log.h"
#include "ECS/RPGComponents.h"
#include "ECS/RPGSystems.h"
#include "ECS/Registry.h"
#include "ECS/Systems.h"
#include "Graphics/Camera.h"
#include "Graphics/Font.h"
#include "Graphics/SpriteBatch.h"
#include "Graphics/Texture.h"
#include "Graphics/TileMap.h"
#include "runapch.h"
#include <cstdlib>
#include <ctime>
#include <memory>

class RunaRPG : public Runa::Application {
public:
  RunaRPG() : Application("Runa", 1280, 720) {}

protected:
  void onInit() override {
    LOG_INFO("=== Runa RPG ===");
    srand(static_cast<unsigned>(time(nullptr)));


    m_registry = std::make_unique<Runa::ECS::EntityRegistry>();
    m_spriteBatch = std::make_unique<Runa::SpriteBatch>(getRenderer());
    m_font = std::make_unique<Runa::Font>(getRenderer(),
                                          "Resources/Fonts/Renogare.ttf", 20);
    m_camera = std::make_unique<Runa::Camera>(getWindow());
    m_inputManager = std::make_unique<Runa::InputManager>(getInput());
    m_inputManager->initialize();


    unsigned char whitePixel[4] = {255, 255, 255, 255};
    m_whitePixelTexture = std::make_unique<Runa::Texture>(getRenderer(), 1, 1, whitePixel);


    setupInput();


    createWorld();


    createPlayer();


    if (m_registry->getRegistry().valid(m_player)) {
      auto& pos = m_registry->getRegistry().get<Runa::ECS::Position>(m_player);
      auto& size = m_registry->getRegistry().get<Runa::ECS::Size>(m_player);
      m_camera->setPosition(pos.x + size.width / 2.0f, pos.y + size.height / 2.0f);
    }


    for (int i = 0; i < 8; ++i) {
      spawnSlime();
    }


    for (int i = 0; i < 10; ++i) {
      spawnPotion();
      spawnCoin();
    }


    createQuestGiver();

    m_gameTime = 0.0f;

    LOG_INFO("RPG initialized!");
    LOG_INFO("Controls:");
    LOG_INFO("  WASD - Move");
    LOG_INFO("  SPACE - Attack");
    LOG_INFO("  I - Toggle Inventory");
    LOG_INFO("  E - Interact with NPCs");
  }

  void setupInput() {

    m_inputManager->bind2DAxis("Gameplay", "Move", SDLK_W, SDLK_S, SDLK_A,
                               SDLK_D);


    m_inputManager->bindKey("Gameplay", "Attack", SDLK_SPACE);
    m_inputManager->bindKey("Gameplay", "Interact", SDLK_E);
    m_inputManager->bindKey("Gameplay", "ToggleInventory", SDLK_I);

    m_inputManager->setActiveContext("Gameplay");
  }

  void createWorld() {

    m_tileMap = std::make_unique<Runa::TileMap>(50, 50, 32);


    for (int y = 0; y < 50; ++y) {
      for (int x = 0; x < 50; ++x) {

        if (x < 5 || x > 45 || y < 5 || y > 45) {
          m_tileMap->setTile(x, y, 2);
        } else if ((x + y) % 3 == 0) {
          m_tileMap->setTile(x, y, 1);
        } else {
          m_tileMap->setTile(x, y, 0);
        }
      }
    }


    for (int i = 0; i < 20; ++i) {
      int x = 10 + (rand() % 30);
      int y = 10 + (rand() % 30);
      m_tileMap->setTile(x, y, 3);
      m_tileMap->setSolidTile(3, true);
    }
  }

  void createPlayer() {
    float startX = 25.0f * 32.0f;
    float startY = 25.0f * 32.0f;

    m_player = m_registry->createEntity(startX, startY);


    auto &reg = m_registry->getRegistry();
    reg.emplace<Runa::ECS::Player>(m_player);
    reg.emplace<Runa::ECS::Size>(m_player, 24.0f, 24.0f);
    reg.emplace<Runa::ECS::AABB>(m_player, 24.0f, 24.0f);
    reg.emplace<Runa::ECS::CameraTarget>(m_player);


    auto &sprite = reg.emplace<Runa::ECS::Sprite>(m_player);
    sprite.spriteSheet = nullptr;
    sprite.tintR = 0.2f;
    sprite.tintG = 0.5f;
    sprite.tintB = 1.0f;
    sprite.tintA = 1.0f;


    auto &health = reg.emplace<Runa::ECS::Health>(m_player);
    health.current = 100.0f;
    health.max = 100.0f;

    auto &combat = reg.emplace<Runa::ECS::Combat>(m_player);
    combat.damage = 15.0f;
    combat.attackRange = 40.0f;
    combat.attackCooldown = 0.5f;

    auto &xp = reg.emplace<Runa::ECS::Experience>(m_player);
    xp.currentXP = 0;
    xp.level = 1;
    xp.xpToNextLevel = 100;

    auto &inv = reg.emplace<Runa::ECS::Inventory>(m_player);
    inv.gold = 0;
    inv.maxSlots = 20;


    reg.emplace<Runa::ECS::Velocity>(m_player, 0.0f, 0.0f);


    reg.emplace<Runa::ECS::PlayerInput>(m_player, 150.0f);
  }

  void spawnSlime() {

    float x = (10 + (rand() % 30)) * 32.0f;
    float y = (10 + (rand() % 30)) * 32.0f;

    auto slime = m_registry->createEntity(x, y);
    auto &reg = m_registry->getRegistry();

    reg.emplace<Runa::ECS::Enemy>(slime);
    reg.emplace<Runa::ECS::Velocity>(slime, 0.0f, 0.0f);
    reg.emplace<Runa::ECS::Size>(slime, 28.0f, 28.0f);
    reg.emplace<Runa::ECS::AABB>(slime, 28.0f, 28.0f);


    auto &sprite = reg.emplace<Runa::ECS::Sprite>(slime);
    sprite.spriteSheet = nullptr;
    sprite.tintR = 0.2f;
    sprite.tintG = 0.8f;
    sprite.tintB = 0.2f;
    sprite.tintA = 1.0f;


    auto &health = reg.emplace<Runa::ECS::Health>(slime);
    health.current = 30.0f;
    health.max = 30.0f;


    auto &combat = reg.emplace<Runa::ECS::Combat>(slime);
    combat.damage = 5.0f;
    combat.attackRange = 30.0f;
    combat.attackCooldown = 1.5f;


    auto &ai = reg.emplace<Runa::ECS::AIController>(slime);
    ai.state = Runa::ECS::AIState::Patrol;
    ai.detectionRange = 180.0f;
    ai.attackRange = 30.0f;
    ai.moveSpeed = 100.0f;
  }

  void spawnPotion() {
    float x = (5 + (rand() % 40)) * 32.0f;
    float y = (5 + (rand() % 40)) * 32.0f;

    auto item = m_registry->createEntity(x, y);
    auto &reg = m_registry->getRegistry();

    reg.emplace<Runa::ECS::ItemEntity>(item);
    reg.emplace<Runa::ECS::Size>(item, 16.0f, 16.0f);


    auto &sprite = reg.emplace<Runa::ECS::Sprite>(item);
    sprite.spriteSheet = nullptr;
    sprite.tintR = 1.0f;
    sprite.tintG = 0.2f;
    sprite.tintB = 0.2f;
    sprite.tintA = 1.0f;


    auto &droppedItem = reg.emplace<Runa::ECS::DroppedItem>(item);
    droppedItem.item.type = Runa::ECS::ItemType::Potion;
    droppedItem.item.name = "Health Potion";
    droppedItem.item.description = "Restores 30 HP";
    droppedItem.item.healAmount = 30.0f;
    droppedItem.item.value = 10;
    droppedItem.collected = false;
  }

  void spawnCoin() {
    float x = (5 + (rand() % 40)) * 32.0f;
    float y = (5 + (rand() % 40)) * 32.0f;

    auto item = m_registry->createEntity(x, y);
    auto &reg = m_registry->getRegistry();

    reg.emplace<Runa::ECS::ItemEntity>(item);
    reg.emplace<Runa::ECS::Size>(item, 12.0f, 12.0f);


    auto &sprite = reg.emplace<Runa::ECS::Sprite>(item);
    sprite.spriteSheet = nullptr;
    sprite.tintR = 1.0f;
    sprite.tintG = 0.9f;
    sprite.tintB = 0.0f;
    sprite.tintA = 1.0f;


    auto &droppedItem = reg.emplace<Runa::ECS::DroppedItem>(item);
    droppedItem.item.type = Runa::ECS::ItemType::Coin;
    droppedItem.item.name = "Gold Coin";
    droppedItem.item.value = 5;
    droppedItem.collected = false;
  }

  void createQuestGiver() {
    float x = 25.0f * 32.0f + 100.0f;
    float y = 25.0f * 32.0f - 100.0f;

    auto npc = m_registry->createEntity(x, y);
    auto &reg = m_registry->getRegistry();

    reg.emplace<Runa::ECS::NPC>(npc);
    reg.emplace<Runa::ECS::Size>(npc, 24.0f, 24.0f);


    auto &sprite = reg.emplace<Runa::ECS::Sprite>(npc);
    sprite.spriteSheet = nullptr;
    sprite.tintR = 0.7f;
    sprite.tintG = 0.2f;
    sprite.tintB = 0.9f;
    sprite.tintA = 1.0f;


    auto &questGiver = reg.emplace<Runa::ECS::QuestGiver>(npc);
    questGiver.npcName = "Village Elder";
    questGiver.dialogueText = "Please help us! Defeat 5 slimes!";
    questGiver.quest.id = "slime_hunter";
    questGiver.quest.title = "Slime Infestation";
    questGiver.quest.description =
        "The village is under attack! Defeat 5 slimes.";
    questGiver.quest.status = Runa::ECS::QuestStatus::NotStarted;
    questGiver.quest.enemiesRequired = 5;
    questGiver.quest.enemiesKilled = 0;
    questGiver.quest.xpReward = 200;
    questGiver.quest.goldReward = 100;
  }

  void onUpdate(float dt) override {
    m_gameTime += dt;


    auto &reg = m_registry->getRegistry();
    if (reg.valid(m_player) && reg.all_of<Runa::ECS::PlayerInput>(m_player)) {
      float moveX = m_inputManager->getActionAxisX("Move");
      float moveY = m_inputManager->getActionAxisY("Move");

      auto &playerInput = reg.get<Runa::ECS::PlayerInput>(m_player);
      auto &vel = reg.get<Runa::ECS::Velocity>(m_player);
      vel.x = moveX * playerInput.speed;
      vel.y = moveY * playerInput.speed;
    }


    if (m_inputManager->isActionPressed("Attack")) {
      if (reg.valid(m_player) && reg.all_of<Runa::ECS::Combat>(m_player)) {
        auto &combat = reg.get<Runa::ECS::Combat>(m_player);
        combat.attackRange = 50.0f;
        LOG_DEBUG("Player attacks!");
      }
    }


    if (m_inputManager->isActionPressed("Interact")) {
      auto questView = reg.view<Runa::ECS::QuestGiver, Runa::ECS::Position>();
      auto playerView = reg.view<Runa::ECS::Player, Runa::ECS::Position>();

      if (playerView.size_hint() != 0) {
        auto playerPos =
            playerView.get<Runa::ECS::Position>(playerView.front());

        for (auto npc : questView) {
          auto npcPos = questView.get<Runa::ECS::Position>(npc);
          float dx = playerPos.x - npcPos.x;
          float dy = playerPos.y - npcPos.y;
          float dist = std::sqrt(dx * dx + dy * dy);

          if (dist < 80.0f) {
            auto &questGiver = questView.get<Runa::ECS::QuestGiver>(npc);
            if (questGiver.quest.status == Runa::ECS::QuestStatus::NotStarted) {
              questGiver.quest.status = Runa::ECS::QuestStatus::InProgress;
              LOG_INFO("Quest started: {}", questGiver.quest.title);
              m_showQuestText = true;
              m_questTextTimer = 3.0f;
            } else if (questGiver.quest.status ==
                       Runa::ECS::QuestStatus::Completed) {
              LOG_INFO("Quest already completed! Thank you, hero!");
            }
          }
        }
      }
    }


    if (m_inputManager->isActionPressed("ToggleInventory")) {
      m_showInventory = !m_showInventory;
    }


    if (m_questTextTimer > 0.0f) {
      m_questTextTimer -= dt;
      if (m_questTextTimer <= 0.0f) {
        m_showQuestText = false;
      }
    }


    Runa::ECS::Systems::updateMovement(reg, dt);
    Runa::ECS::Systems::updateAnimation(reg, dt);
    Runa::ECS::RPGSystems::updateAI(reg, dt);
    Runa::ECS::RPGSystems::updateCombat(reg, dt, m_gameTime);
    Runa::ECS::RPGSystems::updateItemCollection(reg);
    Runa::ECS::RPGSystems::updateQuests(reg);
    Runa::ECS::RPGSystems::updateDamageNumbers(reg, dt);


    if (reg.valid(m_player)) {
      m_camera->followEntity(reg, m_player, 0.1f);
    }
    m_camera->update(dt);


    if (reg.valid(m_player) && reg.all_of<Runa::ECS::Health>(m_player)) {
      auto &health = reg.get<Runa::ECS::Health>(m_player);
      if (health.isDead && !m_gameOver) {
        m_gameOver = true;
        LOG_INFO("Game Over!");
      }
    }


    auto enemyView = reg.view<Runa::ECS::Enemy>();
    int enemyCount = 0;
    for (auto entity : enemyView) {
      enemyCount++;
    }
    if (enemyCount < 3) {
      spawnSlime();
    }
  }

  void onRender() override {
    getRenderer().clear(0.05f, 0.1f, 0.05f, 1.0f);

    m_spriteBatch->begin();


    renderWorld();


    Runa::ECS::Systems::renderSprites(m_registry->getRegistry(), *m_spriteBatch,
                                      *m_camera, m_whitePixelTexture.get());


    Runa::ECS::RPGSystems::renderDamageNumbers(
        m_registry->getRegistry(), *m_spriteBatch, *m_font, *m_camera);


    Runa::ECS::RPGSystems::renderPlayerUI(m_registry->getRegistry(),
                                          *m_spriteBatch, *m_font, 1280, 720);


    if (m_showQuestText) {
      auto texture = m_font->renderText("New Quest: Slime Infestation!",
                                        {255, 255, 100, 255});
      if (texture) {
        m_spriteBatch->draw(*texture, 400, 300);
      }
    }


    if (m_showInventory) {
      renderInventory();
    }


    if (m_gameOver) {
      auto texture = m_font->renderText("GAME OVER", {255, 50, 50, 255});
      if (texture) {
        m_spriteBatch->draw(*texture, 500, 350);
      }
    }

    m_spriteBatch->end();
  }

  void renderWorld() {

    auto bounds = m_camera->getWorldBounds();
    int startX = std::max(0, static_cast<int>(bounds.left / 32));
    int startY = std::max(0, static_cast<int>(bounds.top / 32));
    int endX = std::min(49, static_cast<int>(bounds.right / 32) + 1);
    int endY = std::min(49, static_cast<int>(bounds.bottom / 32) + 1);

    if (m_whitePixelTexture && m_whitePixelTexture->isValid()) {
      for (int y = startY; y <= endY; ++y) {
        for (int x = startX; x <= endX; ++x) {
          int tileIndex = m_tileMap->getTile(x, y);

          int screenX, screenY;

          m_camera->worldToScreen(x * 32.0f + 16.0f, y * 32.0f + 16.0f, screenX, screenY);


          float r = 0.2f, g = 0.6f, b = 0.2f;
          if (tileIndex == 1) {

            r = 0.6f; g = 0.4f; b = 0.2f;
          } else if (tileIndex == 2) {

            r = 0.1f; g = 0.4f; b = 0.1f;
          } else if (tileIndex == 3) {

            r = 0.4f; g = 0.4f; b = 0.4f;
          }


          int drawX = screenX - 16;
          int drawY = screenY - 16;
          // Use first overload with srcWidth/srcHeight, scale will be applied by s_pixelScale
          m_spriteBatch->draw(*m_whitePixelTexture, drawX, drawY, 0, 0, 1, 1, r, g, b, 1.0f, 32.0f / 3.0f, 32.0f / 3.0f);
        }
      }
    }
  }

  void renderInventory() {
    auto &reg = m_registry->getRegistry();
    auto playerView = reg.view<Runa::ECS::Player, Runa::ECS::Inventory>();
    if (playerView.size_hint() == 0)
      return;

    auto &inv = playerView.get<Runa::ECS::Inventory>(playerView.front());


    auto title = m_font->renderText("=== INVENTORY ===", {255, 255, 255, 255});
    if (title) {
      m_spriteBatch->draw(*title, 400, 100);
    }

    int y = 150;
    if (inv.items.empty()) {
      auto empty = m_font->renderText("(Empty)", {150, 150, 150, 255});
      if (empty) {
        m_spriteBatch->draw(*empty, 450, y);
      }
    } else {
      for (const auto &item : inv.items) {
        std::string itemText =
            item.name + " x" + std::to_string(item.stackSize);
        auto texture = m_font->renderText(itemText, {200, 200, 255, 255});
        if (texture) {
          m_spriteBatch->draw(*texture, 420, y);
          y += 30;
        }
      }
    }
  }

  void onShutdown() override { m_inputManager->shutdown(); }

private:
  std::unique_ptr<Runa::ECS::EntityRegistry> m_registry;
  std::unique_ptr<Runa::SpriteBatch> m_spriteBatch;
  std::unique_ptr<Runa::Font> m_font;
  std::unique_ptr<Runa::Camera> m_camera;
  std::unique_ptr<Runa::TileMap> m_tileMap;
  std::unique_ptr<Runa::InputManager> m_inputManager;
  std::unique_ptr<Runa::Texture> m_whitePixelTexture;

  entt::entity m_player;
  float m_gameTime = 0.0f;
  bool m_showInventory = false;
  bool m_gameOver = false;
  bool m_showQuestText = false;
  float m_questTextTimer = 0.0f;
};

int main(int argc, char *argv[]) {
  Runa::Log::init();
  LOG_INFO("Starting Runa RPG...");

  try {
    RunaRPG game;
    game.run();
  } catch (const std::exception &e) {
    LOG_CRITICAL("Fatal error: {}", e.what());
    return 1;
  }

  LOG_INFO("Game exited successfully");
  return 0;
}
