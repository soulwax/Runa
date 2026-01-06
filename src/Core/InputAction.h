/**
 * InputAction.h
 * Defines input actions and action types for the input manager.
 */

#pragma once

#include "../RunaAPI.h"
#include <string>
#include <SDL3/SDL.h>

namespace Runa {

	// Input types supported by the system
	enum class InputType {
		Keyboard,
		MouseButton,
		MouseAxis,
		GamepadButton,
		GamepadAxis,
		GamepadTrigger
	};

	// Input action types
	enum class ActionType {
		Button,  // Binary on/off (jump, fire, etc.)
		Axis1D,  // Single axis value (-1 to 1 or 0 to 1)
		Axis2D   // Two-dimensional input (movement, aim, etc.)
	};

	// Input device identifier
	struct RUNA_API InputDevice {
		enum class Type {
			Keyboard,
			Mouse,
			Gamepad
		};

		Type type = Type::Keyboard;
		int deviceIndex = 0;  // For gamepads (0 = player 1, 1 = player 2, etc.)

		bool operator==(const InputDevice& other) const {
			return type == other.type && deviceIndex == other.deviceIndex;
		}
	};

	// Represents a single input source (key, button, axis, etc.)
	struct RUNA_API InputSource {
		InputType type = InputType::Keyboard;
		int code = 0;  // Keycode, button index, or axis index
		float scale = 1.0f;  // For axes: 1.0 or -1.0 to invert
		float deadZone = 0.15f;  // For analog inputs
		InputDevice device;

		InputSource() = default;

		// Keyboard constructor
		static InputSource keyboard(SDL_Keycode key) {
			InputSource src;
			src.type = InputType::Keyboard;
			src.code = static_cast<int>(key);
			src.device.type = InputDevice::Type::Keyboard;
			return src;
		}

		// Mouse button constructor
		static InputSource mouseButton(int button) {
			InputSource src;
			src.type = InputType::MouseButton;
			src.code = button;
			src.device.type = InputDevice::Type::Mouse;
			return src;
		}

		// Gamepad button constructor
		static InputSource gamepadButton(SDL_GamepadButton button, int playerIndex = 0) {
			InputSource src;
			src.type = InputType::GamepadButton;
			src.code = static_cast<int>(button);
			src.device.type = InputDevice::Type::Gamepad;
			src.device.deviceIndex = playerIndex;
			return src;
		}

		// Gamepad axis constructor
		static InputSource gamepadAxis(SDL_GamepadAxis axis, float scale = 1.0f, int playerIndex = 0) {
			InputSource src;
			src.type = InputType::GamepadAxis;
			src.code = static_cast<int>(axis);
			src.scale = scale;
			src.device.type = InputDevice::Type::Gamepad;
			src.device.deviceIndex = playerIndex;
			return src;
		}
	};

	// Action value - stores the current state of an action
	struct RUNA_API ActionValue {
		ActionType type = ActionType::Button;

		// Button state
		bool buttonDown = false;
		bool buttonPressed = false;  // This frame only
		bool buttonReleased = false;  // This frame only

		// Axis values
		float axis1D = 0.0f;  // -1 to 1 for bidirectional, 0 to 1 for triggers
		float axis2DX = 0.0f;  // -1 to 1
		float axis2DY = 0.0f;  // -1 to 1

		// Helpers
		bool isPressed() const { return buttonPressed; }
		bool isDown() const { return buttonDown; }
		bool isReleased() const { return buttonReleased; }

		float getAxis() const { return axis1D; }
		float getAxisX() const { return axis2DX; }
		float getAxisY() const { return axis2DY; }

		void reset() {
			buttonPressed = false;
			buttonReleased = false;
			if (!buttonDown) {
				axis1D = 0.0f;
				axis2DX = 0.0f;
				axis2DY = 0.0f;
			}
		}
	};

}  // namespace Runa
