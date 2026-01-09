/**
 * main_scene_demo.cpp
 * Scene management system demo showcasing:
 * - Menu → Game transitions
 * - Pause menu overlay
 * - Scene stack management
 */

#include "runapch.h"
#include "Core/Application.h"
#include "Core/Log.h"
#include "Core/SceneManager.h"
#include "Scenes/MenuScene.h"

class SceneDemoApp : public Runa::Application {
public:
  SceneDemoApp() : Application("Runa2 - Scene System Demo", 1280, 720) {}

protected:
  void onInit() override {
    LOG_INFO("=== Runa2 Scene System Demo ===");
    LOG_INFO("Menu: Press SPACE to start game");
    LOG_INFO("Game: Press ESC to pause, WASD to move");
    LOG_INFO("Pause: Press ESC to resume, Q to quit to menu");


    getSceneManager().pushScene(std::make_unique<Runa::MenuScene>(*this));
  }

  void onShutdown() override {
    LOG_INFO("Shutting down scene demo...");
    getSceneManager().clearScenes();
  }
};

int main(int argc, char* argv[]) {

  Runa::Log::init();
  LOG_INFO("Starting Runa2 Scene System Demo");

  try {
    SceneDemoApp app;
    app.run();
  } catch (const std::exception& e) {
    LOG_CRITICAL("Fatal error: {}", e.what());
    return 1;
  }

  LOG_INFO("Scene demo exited successfully");
  return 0;
}
