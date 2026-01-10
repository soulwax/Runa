/**
 * main_game.cpp
 * Clean game implementation - uses TestScene for level logic
 */

#include "Core/Application.h"
#include "Core/Log.h"
#include "Core/SceneManager.h"
#include "Scenes/TestScene.h"
#include "runapch.h"

class Game : public Runa::Application {
public:
    Game() : Application("Runa2 Game", 2560, 1440) {}

protected:
    void onInit() override {
        LOG_INFO("=== Runa2 Game ===");

        // Push TestScene as the initial scene
        getSceneManager().pushScene(std::make_unique<Runa::TestScene>(*this));

        LOG_INFO("Game initialized with TestScene");
    }
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
