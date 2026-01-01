// Minimal test to verify grass texture rendering
#include "Core/Application.h"
#include "Core/Log.h"
#include "Graphics/SpriteBatch.h"
#include "Graphics/Texture.h"
#include <memory>

class GrassTest : public Runa::Application {
public:
    GrassTest() : Application("Grass Texture Test", 1280, 720) {}

protected:
    void onInit() override {
        LOG_INFO("=== Grass Texture Test ===");

        m_spriteBatch = std::make_unique<Runa::SpriteBatch>(getRenderer());

        try {
            m_grassTexture = std::make_unique<Runa::Texture>(
                getRenderer(), "Resources/mystic_woods_2.2/sprites/tilesets/grass.png");
            LOG_INFO("Grass texture loaded!");
        } catch (const std::exception& e) {
            LOG_ERROR("Failed to load grass: {}", e.what());
        }
    }

    void onUpdate(float dt) override {}

    void onRender() override {
        getRenderer().clear(0.1f, 0.15f, 0.2f, 1.0f);

        if (!m_grassTexture)
            return;

        m_spriteBatch->begin();

        // Draw grass texture tiled across screen
        int tileSize = 16;
        for (int y = 0; y < 720 / tileSize; ++y) {
            for (int x = 0; x < 1280 / tileSize; ++x) {
                m_spriteBatch->draw(*m_grassTexture, x * tileSize, y * tileSize);
            }
        }

        m_spriteBatch->end();
    }

    void onShutdown() override {}

private:
    std::unique_ptr<Runa::SpriteBatch> m_spriteBatch;
    std::unique_ptr<Runa::Texture> m_grassTexture;
};

int main(int argc, char* argv[]) {
    try {
        Runa::Log::init();
        auto app = std::make_unique<GrassTest>();
        app->run();
        return 0;
    } catch (const std::exception& e) {
        try {
            LOG_CRITICAL("Fatal error: {}", e.what());
        } catch (...) {
            std::cerr << "Fatal error: " << e.what() << std::endl;
        }
        return 1;
    }
}
