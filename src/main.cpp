// File: src/main.cpp

#include "Core/Application.h"
#include <iostream>
#include <memory>

class GameApp : public Runa::Application {
public:
    GameApp() : Application("Runa2 - Game Engine", 1280, 720) {}

protected:
    void onInit() override {
        std::cout << "Game initialized!" << std::endl;
        std::cout << "Press ESC or close window to exit." << std::endl;
    }

    void onUpdate(float deltaTime) override {
        // Game logic updates here
    }

    void onRender() override {
        // Clear screen with a dark blue color
        getRenderer().clear(0.1f, 0.1f, 0.2f, 1.0f);

        // Rendering code here
    }

    void onShutdown() override {
        std::cout << "Game shutting down..." << std::endl;
    }
};

int main(int argc, char* argv[]) {
    try {
        auto app = std::make_unique<GameApp>();
        app->run();
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }
}
