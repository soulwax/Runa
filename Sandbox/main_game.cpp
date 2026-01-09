/**
 * main_game.cpp
 * Clean game implementation - blank canvas with FPS display and meadow background
 */

#include "Core/Application.h"
#include "Core/Log.h"
#include "Graphics/Font.h"
#include "Graphics/SpriteBatch.h"
#include "Graphics/SpriteSheet.h"
#include "runapch.h"
#include <memory>
#include <chrono>
#include <string>
#include <cstdlib>
#include <ctime>
#include <vector>

class Game : public Runa::Application {
public:
    Game() : Application("Runa2 Game", 1280, 720) {}

protected:
    void onInit() override {
        LOG_INFO("=== Runa2 Game ===");
        srand(static_cast<unsigned>(time(nullptr)));


        m_spriteBatch = std::make_unique<Runa::SpriteBatch>(getRenderer());


        m_font = std::make_unique<Runa::Font>(getRenderer(),
                                              "Resources/Fonts/Renogare.ttf", 24);

        if (!m_font->isValid()) {
            LOG_WARN("Failed to load font, FPS display may not work");
        }


        try {
            m_baseGrassSheet = std::make_unique<Runa::SpriteSheet>(getRenderer(),
                                                                   "Resources/SpiteSheets/grass.png");

            m_baseGrassSheet->addSprite("grass_base", 0, 0, 16, 16);
            LOG_INFO("Base grass sprite sheet loaded");
        } catch (const std::exception& e) {
            LOG_ERROR("Failed to load base grass sprite sheet: {}", e.what());
        }


        try {
            m_decorGrassSheet = std::make_unique<Runa::SpriteSheet>(getRenderer(),
                                                                     "Resources/SpiteSheets/decor-grass.png");


            m_decorGrassSheet->createGrid("decor_grass", 16, 16, 4, 5);
            LOG_INFO("Decorative grass sprite sheet loaded with {} sprites", m_decorGrassSheet->getSpriteNames().size());
        } catch (const std::exception& e) {
            LOG_ERROR("Failed to load decorative grass sprite sheet: {}", e.what());
        }


        try {
            m_objectsSheet = std::make_unique<Runa::SpriteSheet>(getRenderer(),
                                                                  "Resources/SpiteSheets/objects.png");


            m_objectsSheet->createGrid("object", 16, 16);
            LOG_INFO("Objects sprite sheet loaded with {} sprites", m_objectsSheet->getSpriteNames().size());
        } catch (const std::exception& e) {
            LOG_ERROR("Failed to load objects sprite sheet: {}", e.what());
        }


        generateMeadow();


        m_lastFPSUpdate = std::chrono::steady_clock::now();
        m_displayedFPS = 0;
        m_frameCount = 0;

        LOG_INFO("Game initialized");
    }

    void generateMeadow() {
        const int tileSize = 16;
        const int meadowWidth = 80;
        const int meadowHeight = 45;


        m_baseMeadowTiles.clear();
        m_baseMeadowTiles.reserve(meadowWidth * meadowHeight);

        for (int y = 0; y < meadowHeight; ++y) {
            for (int x = 0; x < meadowWidth; ++x) {
                m_baseMeadowTiles.push_back({
                    x * tileSize,
                    y * tileSize
                });
            }
        }


        m_decorMeadowTiles.clear();

        auto decorGrassNames = m_decorGrassSheet ? m_decorGrassSheet->getSpriteNames() : std::vector<std::string>();
        if (decorGrassNames.empty()) {
            LOG_WARN("No decorative grass sprites available");
        } else {

            const int maxDecorTiles = std::min(16, static_cast<int>(decorGrassNames.size()));


            for (int y = 0; y < meadowHeight; ++y) {
                for (int x = 0; x < meadowWidth; ++x) {
                    if (rand() % 5 == 0) {
                        int tileIndex = rand() % maxDecorTiles;
                        m_decorMeadowTiles.push_back({
                            x * tileSize,
                            y * tileSize,
                            decorGrassNames[tileIndex]
                        });
                    }
                }
            }
        }


        m_decorObjects.clear();

        auto objectNames = m_objectsSheet ? m_objectsSheet->getSpriteNames() : std::vector<std::string>();
        if (objectNames.empty()) {
            LOG_WARN("No object sprites available");
        } else {

            const int maxObjects = static_cast<int>(objectNames.size());

            for (int y = 0; y < meadowHeight; ++y) {
                for (int x = 0; x < meadowWidth; ++x) {
                    if (rand() % 100 == 0) {
                        int objectIndex = rand() % maxObjects;
                        m_decorObjects.push_back({
                            x * tileSize,
                            y * tileSize,
                            objectNames[objectIndex]
                        });
                    }
                }
            }
        }

        LOG_INFO("Generated meadow: {} base tiles, {} decorative grass tiles, {} decorative objects",
                 m_baseMeadowTiles.size(), m_decorMeadowTiles.size(), m_decorObjects.size());
    }

    void onUpdate(float deltaTime) override {

        m_frameCount++;


        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
            now - m_lastFPSUpdate).count();

        if (elapsed >= 1000) {
            m_displayedFPS = m_frameCount;
            m_frameCount = 0;
            m_lastFPSUpdate = now;
        }
    }

    void onRender() override {


        getRenderer().clear(0.2f, 0.4f, 0.6f, 1.0f);

        m_spriteBatch->begin();


        if (m_baseGrassSheet) {
            const auto* baseSprite = m_baseGrassSheet->getSprite("grass_base");
            if (baseSprite && !baseSprite->frames.empty()) {
                const auto& baseFrame = baseSprite->frames[0];
                for (const auto& tile : m_baseMeadowTiles) {
                    m_spriteBatch->draw(m_baseGrassSheet->getTexture(),
                                       tile.x, tile.y,
                                       baseFrame.x, baseFrame.y,
                                       baseFrame.width, baseFrame.height);
                }
            }
        }


        if (m_decorGrassSheet) {
            for (const auto& tile : m_decorMeadowTiles) {
                const auto* sprite = m_decorGrassSheet->getSprite(tile.spriteName);
                if (sprite && !sprite->frames.empty()) {
                    const auto& frame = sprite->frames[0];
                    m_spriteBatch->draw(m_decorGrassSheet->getTexture(),
                                       tile.x, tile.y,
                                       frame.x, frame.y,
                                       frame.width, frame.height);
                }
            }
        }


        if (m_objectsSheet) {
            for (const auto& obj : m_decorObjects) {
                const auto* sprite = m_objectsSheet->getSprite(obj.spriteName);
                if (sprite && !sprite->frames.empty()) {
                    const auto& frame = sprite->frames[0];
                    m_spriteBatch->draw(m_objectsSheet->getTexture(),
                                       obj.x, obj.y,
                                       frame.x, frame.y,
                                       frame.width, frame.height);
                }
            }
        }


        if (m_font && m_font->isValid()) {
            std::string fpsText = "FPS: " + std::to_string(m_displayedFPS);
            SDL_Color white = {255, 255, 255, 255};
            auto fpsTexture = m_font->renderText(fpsText, white);

            if (fpsTexture) {

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
    std::unique_ptr<Runa::SpriteSheet> m_baseGrassSheet;
    std::unique_ptr<Runa::SpriteSheet> m_decorGrassSheet;
    std::unique_ptr<Runa::SpriteSheet> m_objectsSheet;


    struct BaseMeadowTile {
        int x, y;
    };
    std::vector<BaseMeadowTile> m_baseMeadowTiles;


    struct DecorMeadowTile {
        int x, y;
        std::string spriteName;
    };
    std::vector<DecorMeadowTile> m_decorMeadowTiles;


    struct DecorObject {
        int x, y;
        std::string spriteName;
    };
    std::vector<DecorObject> m_decorObjects;


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
