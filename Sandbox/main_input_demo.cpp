// File: Sandbox/main_input_demo.cpp

/**
 * main_input_demo.cpp
 * Comprehensive demonstration of the InputManager system featuring:
 * - Action-based input
 * - Multiple input contexts
 * - Keyboard + Gamepad support
 * - 2D movement with WASD/Arrow keys + Left stick
 * - Action buttons with multiple bindings
 * - Context switching
 */

#include "runapch.h"
#include "Core/Application.h"
#include "Core/Log.h"
#include "Core/InputManager.h"
#include "Graphics/SpriteBatch.h"
#include "Graphics/Font.h"
#include "Graphics/Texture.h"
#include <memory>
#include <sstream>
#include <iomanip>

class InputDemoApp : public Runa::Application {
public:
	InputDemoApp() : Application("Runa2 - Input Manager Demo", 1280, 720) {}

protected:
	void onInit() override {
		LOG_INFO("=== Runa2 Input Manager Demo ===");


		m_inputManager = std::make_unique<Runa::InputManager>(getInput());
		m_inputManager->initialize();


		m_spriteBatch = std::make_unique<Runa::SpriteBatch>(getRenderer());
		m_font = std::make_unique<Runa::Font>(getRenderer(), "Resources/Fonts/Renogare.ttf", 24);


		m_playerX = 640.0f;
		m_playerY = 360.0f;


		setupGameplayContext();
		setupMenuContext();


		m_inputManager->setActiveContext("Gameplay");
		m_currentContext = "Gameplay";

		LOG_INFO("Input Demo initialized");


		m_inputManager->saveBindings("Resources/input_bindings.json");
		LOG_INFO("Controls:");
		LOG_INFO("  WASD / Arrow Keys / Left Stick - Move");
		LOG_INFO("  Space / A Button - Jump");
		LOG_INFO("  E / B Button - Interact");
		LOG_INFO("  Tab - Switch to Menu context");
		LOG_INFO("  ESC - Toggle pause");
	}

	void setupGameplayContext() {

		auto* gameplay = m_inputManager->createContext("Gameplay");


		m_inputManager->bind2DAxis("Gameplay", "Move",
		                            SDLK_W, SDLK_S, SDLK_A, SDLK_D);
		m_inputManager->bind2DAxis("Gameplay", "Move",
		                            SDLK_UP, SDLK_DOWN, SDLK_LEFT, SDLK_RIGHT);


		auto* moveBinding = gameplay->getBindingSet().getBinding("Move");
		if (moveBinding) {
			moveBinding->addSource(Runa::InputSource::gamepadAxis(SDL_GAMEPAD_AXIS_LEFTX, 1.0f, 0));
			moveBinding->addSource(Runa::InputSource::gamepadAxis(SDL_GAMEPAD_AXIS_LEFTY, 1.0f, 0));
		}


		m_inputManager->bindKey("Gameplay", "Jump", SDLK_SPACE);
		m_inputManager->bindGamepadButton("Gameplay", "Jump", SDL_GAMEPAD_BUTTON_SOUTH, 0);


		m_inputManager->bindKey("Gameplay", "Interact", SDLK_E);
		m_inputManager->bindGamepadButton("Gameplay", "Interact", SDL_GAMEPAD_BUTTON_EAST, 0);


		m_inputManager->bindKey("Gameplay", "Fire", SDLK_RETURN);
		auto* fireBinding = gameplay->getBindingSet().addBinding("Fire", Runa::ActionType::Button);
		fireBinding->addSource(Runa::InputSource::mouseButton(SDL_BUTTON_LEFT));


		m_inputManager->bindKey("Gameplay", "Pause", SDLK_ESCAPE);


		m_inputManager->bindKey("Gameplay", "SaveBindings", SDLK_F5);
		m_inputManager->bindKey("Gameplay", "LoadBindings", SDLK_F6);
		m_inputManager->bindGamepadButton("Gameplay", "Pause", SDL_GAMEPAD_BUTTON_START, 0);
		m_inputManager->bindGamepadButton("Gameplay", "Pause", SDL_GAMEPAD_BUTTON_START, 0);


		m_inputManager->bindKey("Gameplay", "SwitchContext", SDLK_TAB);
	}

	void setupMenuContext() {

		m_inputManager->createContext("Menu");


		m_inputManager->bindKey("Menu", "Up", SDLK_W);
		m_inputManager->bindKey("Menu", "Up", SDLK_UP);
		m_inputManager->bindGamepadButton("Menu", "Up", SDL_GAMEPAD_BUTTON_DPAD_UP, 0);

		m_inputManager->bindKey("Menu", "Down", SDLK_S);
		m_inputManager->bindKey("Menu", "Down", SDLK_DOWN);
		m_inputManager->bindGamepadButton("Menu", "Down", SDL_GAMEPAD_BUTTON_DPAD_DOWN, 0);

		m_inputManager->bindKey("Menu", "Select", SDLK_RETURN);
		m_inputManager->bindKey("Menu", "Select", SDLK_SPACE);
		m_inputManager->bindGamepadButton("Menu", "Select", SDL_GAMEPAD_BUTTON_SOUTH, 0);

		m_inputManager->bindKey("Menu", "Back", SDLK_ESCAPE);


		m_inputManager->bindKey("Menu", "SaveBindings", SDLK_F5);
		m_inputManager->bindKey("Menu", "LoadBindings", SDLK_F6);
		m_inputManager->bindGamepadButton("Menu", "Back", SDL_GAMEPAD_BUTTON_EAST, 0);
		m_inputManager->bindGamepadButton("Menu", "Back", SDL_GAMEPAD_BUTTON_EAST, 0);


		m_inputManager->bindKey("Menu", "SwitchContext", SDLK_TAB);
	}

	void onUpdate(float dt) override {

		if (m_inputManager->isActionPressed("SaveBindings")) {
			if (m_inputManager->saveBindings("Resources/input_bindings.json")) {
				LOG_INFO("Bindings saved successfully!");
				m_saveMessage = "Bindings Saved!";
				m_saveMessageTimer = 2.0f;
			}
		}

		if (m_inputManager->isActionPressed("LoadBindings")) {
			if (m_inputManager->loadBindings("Resources/input_bindings.json")) {
				LOG_INFO("Bindings loaded successfully!");
				m_saveMessage = "Bindings Loaded!";
				m_saveMessageTimer = 2.0f;
			}
		}


		if (m_saveMessageTimer > 0.0f) {
			m_saveMessageTimer -= dt;
		}


		if (m_inputManager->isActionPressed("SwitchContext")) {
			if (m_currentContext == "Gameplay") {
				m_inputManager->setActiveContext("Menu");
				m_currentContext = "Menu";
				LOG_INFO("Switched to Menu context");
			} else {
				m_inputManager->setActiveContext("Gameplay");
				m_currentContext = "Gameplay";
				LOG_INFO("Switched to Gameplay context");
			}
		}

		if (m_currentContext == "Gameplay") {
			updateGameplay(dt);
		} else if (m_currentContext == "Menu") {
			updateMenu(dt);
		}
	}

	void updateGameplay(float dt) {

		float moveX = m_inputManager->getActionAxisX("Move");
		float moveY = m_inputManager->getActionAxisY("Move");


		const float speed = 300.0f;
		m_playerX += moveX * speed * dt;
		m_playerY += moveY * speed * dt;


		m_playerX = std::clamp(m_playerX, 25.0f, 1255.0f);
		m_playerY = std::clamp(m_playerY, 25.0f, 695.0f);


		if (m_inputManager->isActionPressed("Jump")) {
			LOG_INFO("Jump!");
			m_jumpTimer = 0.3f;
		}

		if (m_inputManager->isActionPressed("Interact")) {
			LOG_INFO("Interact!");
		}

		if (m_inputManager->isActionDown("Fire")) {
			m_fireActive = true;
		} else {
			m_fireActive = false;
		}

		if (m_inputManager->isActionPressed("Pause")) {
			m_paused = !m_paused;
			LOG_INFO("Paused: {}", m_paused);
		}


		if (m_jumpTimer > 0.0f) {
			m_jumpTimer -= dt;
		}
	}

	void updateMenu(float dt) {
		if (m_inputManager->isActionPressed("Up")) {
			m_menuSelection = (m_menuSelection - 1 + 3) % 3;
		}

		if (m_inputManager->isActionPressed("Down")) {
			m_menuSelection = (m_menuSelection + 1) % 3;
		}

		if (m_inputManager->isActionPressed("Select")) {
			LOG_INFO("Menu: Selected option {}", m_menuSelection);
		}

		if (m_inputManager->isActionPressed("Back")) {
			m_inputManager->setActiveContext("Gameplay");
			m_currentContext = "Gameplay";
			LOG_INFO("Switched to Gameplay context");
		}
	}

	void onRender() override {
		getRenderer().clear(0.1f, 0.1f, 0.15f, 1.0f);

		m_spriteBatch->begin();

		if (m_currentContext == "Gameplay") {
			renderGameplay();
		} else if (m_currentContext == "Menu") {
			renderMenu();
		}


		renderInputInfo();

		m_spriteBatch->end();
	}

	void renderGameplay() {

		SDL_Color playerColor = m_fireActive ? SDL_Color{255, 100, 100, 255} : SDL_Color{100, 200, 255, 255};
		if (m_jumpTimer > 0.0f) {
			playerColor = {255, 255, 100, 255};
		}




		if (m_paused) {
			auto pauseText = m_font->renderText("PAUSED", {255, 255, 255, 255});
			if (pauseText) {
				m_spriteBatch->draw(*pauseText, 540, 300);
			}
		}
	}

	void renderMenu() {
		const char* menuItems[] = {"Start Game", "Options", "Quit"};

		for (int i = 0; i < 3; ++i) {
			SDL_Color color = (i == m_menuSelection) ? SDL_Color{255, 255, 0, 255} : SDL_Color{200, 200, 200, 255};
			auto text = m_font->renderText(menuItems[i], color);
			if (text) {
				m_spriteBatch->draw(*text, 500, 250 + i * 50);
			}
		}

		auto titleText = m_font->renderText("MENU", {255, 255, 255, 255});
		if (titleText) {
			m_spriteBatch->draw(*titleText, 580, 150);
		}
	}

	void renderInputInfo() {
		int y = 20;
		auto renderLine = [&](const std::string& text, SDL_Color color = {255, 255, 255, 255}) {
			auto texture = m_font->renderText(text, color);
			if (texture) {
				m_spriteBatch->draw(*texture, 20, y);
				y += 30;
			}
		};


		std::stringstream ss;
		ss << "Context: " << m_currentContext;
		renderLine(ss.str(), {100, 255, 100, 255});


		int gamepadCount = m_inputManager->getGamepadManager().getConnectedGamepadCount();
		ss.str("");
		ss << "Gamepads: " << gamepadCount;
		renderLine(ss.str());

		if (m_currentContext == "Gameplay") {

			float moveX = m_inputManager->getActionAxisX("Move");
			float moveY = m_inputManager->getActionAxisY("Move");
			ss.str("");
			ss << "Move: (" << std::fixed << std::setprecision(2) << moveX << ", " << moveY << ")";
			renderLine(ss.str());


			ss.str("");
			ss << "Player: (" << static_cast<int>(m_playerX) << ", " << static_cast<int>(m_playerY) << ")";
			renderLine(ss.str());


			std::string actions;
			if (m_inputManager->isActionDown("Jump")) actions += "Jump ";
			if (m_inputManager->isActionDown("Interact")) actions += "Interact ";
			if (m_fireActive) actions += "Fire ";
			if (m_paused) actions += "PAUSED ";
			if (!actions.empty()) {
				renderLine("Actions: " + actions, {255, 255, 100, 255});
			}
		}


		y = 680;
		renderLine("TAB - Switch Context | F5 - Save | F6 - Load | ESC - Pause/Back");


		if (m_saveMessageTimer > 0.0f) {
			auto msgText = m_font->renderText(m_saveMessage, {100, 255, 100, 255});
			if (msgText) {
				m_spriteBatch->draw(*msgText, 500, 600);
			}
		}
	}

	void onShutdown() override {
		m_inputManager->shutdown();
	}

private:
	std::unique_ptr<Runa::InputManager> m_inputManager;
	std::unique_ptr<Runa::SpriteBatch> m_spriteBatch;
	std::unique_ptr<Runa::Font> m_font;


	std::string m_currentContext;
	float m_playerX = 640.0f;
	float m_playerY = 360.0f;
	float m_jumpTimer = 0.0f;
	bool m_fireActive = false;
	bool m_paused = false;
	int m_menuSelection = 0;
	std::string m_saveMessage;
	float m_saveMessageTimer = 0.0f;
};

int main(int argc, char* argv[]) {
	Runa::Log::init();
	LOG_INFO("Starting Runa2 Input Manager Demo");

	try {
		InputDemoApp app;
		app.run();
	} catch (const std::exception& e) {
		LOG_CRITICAL("Fatal error: {}", e.what());
		return 1;
	}

	LOG_INFO("Input demo exited successfully");
	return 0;
}
