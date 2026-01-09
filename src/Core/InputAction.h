/**
 * InputAction.h
 * Defines input actions and action types for the input manager.
 */

#ifndef RUNA_CORE_INPUTACTION_H
#define RUNA_CORE_INPUTACTION_H

#include "../RunaAPI.h"
#include <string>
#include <SDL3/SDL.h>

namespace Runa {


	enum class InputType {
		Keyboard,
		MouseButton,
		MouseAxis,
		GamepadButton,
		GamepadAxis,
		GamepadTrigger
	};


	enum class ActionType {
		Button,
		Axis1D,
		Axis2D
	};


	struct RUNA_API InputDevice {
		enum class Type {
			Keyboard,
			Mouse,
			Gamepad
		};

		Type type = Type::Keyboard;
		int deviceIndex = 0;

		bool operator==(const InputDevice& other) const {
			return type == other.type && deviceIndex == other.deviceIndex;
		}
	};


	struct RUNA_API InputSource {
		InputType type = InputType::Keyboard;
		int code = 0;
		float scale = 1.0f;
		float deadZone = 0.15f;
		InputDevice device;

		InputSource() = default;


		static InputSource keyboard(SDL_Keycode key) {
			InputSource src;
			src.type = InputType::Keyboard;
			src.code = static_cast<int>(key);
			src.device.type = InputDevice::Type::Keyboard;
			return src;
		}


		static InputSource mouseButton(int button) {
			InputSource src;
			src.type = InputType::MouseButton;
			src.code = button;
			src.device.type = InputDevice::Type::Mouse;
			return src;
		}


		static InputSource gamepadButton(SDL_GamepadButton button, int playerIndex = 0) {
			InputSource src;
			src.type = InputType::GamepadButton;
			src.code = static_cast<int>(button);
			src.device.type = InputDevice::Type::Gamepad;
			src.device.deviceIndex = playerIndex;
			return src;
		}


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


	struct RUNA_API ActionValue {
		ActionType type = ActionType::Button;


		bool buttonDown = false;
		bool buttonPressed = false;
		bool buttonReleased = false;


		float axis1D = 0.0f;
		float axis2DX = 0.0f;
		float axis2DY = 0.0f;


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

}

#endif
