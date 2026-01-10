// File: src/Core/GamepadManager.cpp

/**
 * GamepadManager.cpp
 * Implementation of gamepad/controller support.
 */

#include "runapch.h"
#include "GamepadManager.h"
#include "Log.h"
#include <algorithm>
#include <cmath>

namespace Runa {

	GamepadManager::GamepadManager() {
	}

	GamepadManager::~GamepadManager() {
		shutdown();
	}

	void GamepadManager::initialize() {
		LOG_INFO("Initializing GamepadManager...");


		int numJoysticks = 0;
		SDL_JoystickID* joysticks = SDL_GetJoysticks(&numJoysticks);

		if (joysticks) {
			for (int i = 0; i < numJoysticks; ++i) {
				if (SDL_IsGamepad(joysticks[i])) {
					addGamepad(joysticks[i]);
				}
			}
			SDL_free(joysticks);
		}

		LOG_INFO("GamepadManager initialized with {} gamepad(s)", m_gamepads.size());
	}

	void GamepadManager::shutdown() {
		LOG_INFO("Shutting down GamepadManager...");

		for (auto& [id, state] : m_gamepads) {
			if (state.handle) {
				SDL_CloseGamepad(state.handle);
				state.handle = nullptr;
			}
		}
		m_gamepads.clear();
		m_nextPlayerIndex = 0;
	}

	void GamepadManager::beginFrame() {

		for (auto& [id, state] : m_gamepads) {
			state.reset();
		}
	}

	void GamepadManager::update(const SDL_Event& event) {
		switch (event.type) {
			case SDL_EVENT_GAMEPAD_ADDED:
				addGamepad(event.gdevice.which);
				break;

			case SDL_EVENT_GAMEPAD_REMOVED:
				removeGamepad(event.gdevice.which);
				break;

			case SDL_EVENT_GAMEPAD_BUTTON_DOWN: {
				auto it = m_gamepads.find(event.gbutton.which);
				if (it != m_gamepads.end()) {
					int button = static_cast<int>(event.gbutton.button);
					if (button >= 0 && button < SDL_GAMEPAD_BUTTON_COUNT) {
						it->second.buttons[button] = true;
						it->second.buttonsPressed[button] = true;
					}
				}
				break;
			}

			case SDL_EVENT_GAMEPAD_BUTTON_UP: {
				auto it = m_gamepads.find(event.gbutton.which);
				if (it != m_gamepads.end()) {
					int button = static_cast<int>(event.gbutton.button);
					if (button >= 0 && button < SDL_GAMEPAD_BUTTON_COUNT) {
						it->second.buttons[button] = false;
						it->second.buttonsReleased[button] = true;
					}
				}
				break;
			}

			case SDL_EVENT_GAMEPAD_AXIS_MOTION: {
				auto it = m_gamepads.find(event.gaxis.which);
				if (it != m_gamepads.end()) {
					int axis = static_cast<int>(event.gaxis.axis);
					if (axis >= 0 && axis < SDL_GAMEPAD_AXIS_COUNT) {

						float value = event.gaxis.value / 32767.0f;


						bool isTrigger = (axis == SDL_GAMEPAD_AXIS_LEFT_TRIGGER ||
						                  axis == SDL_GAMEPAD_AXIS_RIGHT_TRIGGER);
						float deadZone = isTrigger ? it->second.triggerDeadZone : it->second.stickDeadZone;
						value = applyDeadZone(value, deadZone);

						it->second.axes[axis] = value;
					}
				}
				break;
			}

			default:
				break;
		}
	}

	bool GamepadManager::isButtonDown(int playerIndex, SDL_GamepadButton button) const {
		const GamepadState* state = getGamepad(playerIndex);
		if (!state || !state->connected) return false;

		int idx = static_cast<int>(button);
		if (idx < 0 || idx >= SDL_GAMEPAD_BUTTON_COUNT) return false;

		return state->buttons[idx];
	}

	bool GamepadManager::isButtonPressed(int playerIndex, SDL_GamepadButton button) const {
		const GamepadState* state = getGamepad(playerIndex);
		if (!state || !state->connected) return false;

		int idx = static_cast<int>(button);
		if (idx < 0 || idx >= SDL_GAMEPAD_BUTTON_COUNT) return false;

		return state->buttonsPressed[idx];
	}

	bool GamepadManager::isButtonReleased(int playerIndex, SDL_GamepadButton button) const {
		const GamepadState* state = getGamepad(playerIndex);
		if (!state || !state->connected) return false;

		int idx = static_cast<int>(button);
		if (idx < 0 || idx >= SDL_GAMEPAD_BUTTON_COUNT) return false;

		return state->buttonsReleased[idx];
	}

	float GamepadManager::getAxis(int playerIndex, SDL_GamepadAxis axis) const {
		const GamepadState* state = getGamepad(playerIndex);
		if (!state || !state->connected) return 0.0f;

		int idx = static_cast<int>(axis);
		if (idx < 0 || idx >= SDL_GAMEPAD_AXIS_COUNT) return 0.0f;

		return state->axes[idx];
	}

	float GamepadManager::getLeftStickX(int playerIndex) const {
		return getAxis(playerIndex, SDL_GAMEPAD_AXIS_LEFTX);
	}

	float GamepadManager::getLeftStickY(int playerIndex) const {
		return getAxis(playerIndex, SDL_GAMEPAD_AXIS_LEFTY);
	}

	float GamepadManager::getRightStickX(int playerIndex) const {
		return getAxis(playerIndex, SDL_GAMEPAD_AXIS_RIGHTX);
	}

	float GamepadManager::getRightStickY(int playerIndex) const {
		return getAxis(playerIndex, SDL_GAMEPAD_AXIS_RIGHTY);
	}

	float GamepadManager::getLeftTrigger(int playerIndex) const {
		return getAxis(playerIndex, SDL_GAMEPAD_AXIS_LEFT_TRIGGER);
	}

	float GamepadManager::getRightTrigger(int playerIndex) const {
		return getAxis(playerIndex, SDL_GAMEPAD_AXIS_RIGHT_TRIGGER);
	}

	bool GamepadManager::isGamepadConnected(int playerIndex) const {
		const GamepadState* state = getGamepad(playerIndex);
		return state && state->connected;
	}

	int GamepadManager::getConnectedGamepadCount() const {
		int count = 0;
		for (const auto& [id, state] : m_gamepads) {
			if (state.connected) ++count;
		}
		return count;
	}

	void GamepadManager::setStickDeadZone(int playerIndex, float deadZone) {
		GamepadState* state = getGamepad(playerIndex);
		if (state) {
			state->stickDeadZone = std::clamp(deadZone, 0.0f, 1.0f);
		}
	}

	void GamepadManager::setTriggerDeadZone(int playerIndex, float deadZone) {
		GamepadState* state = getGamepad(playerIndex);
		if (state) {
			state->triggerDeadZone = std::clamp(deadZone, 0.0f, 1.0f);
		}
	}

	GamepadState* GamepadManager::getGamepad(int playerIndex) {
		for (auto& [id, state] : m_gamepads) {
			if (state.playerIndex == playerIndex) {
				return &state;
			}
		}
		return nullptr;
	}

	const GamepadState* GamepadManager::getGamepad(int playerIndex) const {
		for (const auto& [id, state] : m_gamepads) {
			if (state.playerIndex == playerIndex) {
				return &state;
			}
		}
		return nullptr;
	}

	void GamepadManager::addGamepad(SDL_JoystickID deviceID) {
		SDL_Gamepad* gamepad = SDL_OpenGamepad(deviceID);
		if (!gamepad) {
			LOG_ERROR("Failed to open gamepad {}: {}", deviceID, SDL_GetError());
			return;
		}

		SDL_JoystickID instanceID = SDL_GetGamepadID(gamepad);
		const char* name = SDL_GetGamepadName(gamepad);

		GamepadState state;
		state.handle = gamepad;
		state.instanceID = instanceID;
		state.playerIndex = m_nextPlayerIndex++;
		state.connected = true;

		m_gamepads[instanceID] = state;

		LOG_INFO("Gamepad connected: {} (Player {})", name ? name : "Unknown", state.playerIndex + 1);
	}

	void GamepadManager::removeGamepad(SDL_JoystickID instanceID) {
		auto it = m_gamepads.find(instanceID);
		if (it != m_gamepads.end()) {
			LOG_INFO("Gamepad disconnected (Player {})", it->second.playerIndex + 1);

			if (it->second.handle) {
				SDL_CloseGamepad(it->second.handle);
			}
			m_gamepads.erase(it);
		}
	}

	float GamepadManager::applyDeadZone(float value, float deadZone) const {
		if (std::abs(value) < deadZone) {
			return 0.0f;
		}


		float sign = (value > 0) ? 1.0f : -1.0f;
		float absValue = std::abs(value);
		float remapped = (absValue - deadZone) / (1.0f - deadZone);
		return sign * std::clamp(remapped, 0.0f, 1.0f);
	}

}
