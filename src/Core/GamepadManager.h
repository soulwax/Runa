/**
 * GamepadManager.h
 * Manages gamepad/controller input with SDL3.
 */

#ifndef RUNA_CORE_GAMEPADMANAGER_H
#define RUNA_CORE_GAMEPADMANAGER_H

#include "../RunaAPI.h"
#include <SDL3/SDL.h>
#include <unordered_map>
#include <vector>

namespace Runa {

// Gamepad state for a single controller
struct RUNA_API GamepadState {
  SDL_Gamepad *handle = nullptr;
  SDL_JoystickID instanceID = 0;
  int playerIndex = 0;
  bool connected = false;

  // Button states
  bool buttons[SDL_GAMEPAD_BUTTON_COUNT] = {};
  bool buttonsPressed[SDL_GAMEPAD_BUTTON_COUNT] = {};
  bool buttonsReleased[SDL_GAMEPAD_BUTTON_COUNT] = {};

  // Axis values (-1 to 1, normalized)
  float axes[SDL_GAMEPAD_AXIS_COUNT] = {};

  // Dead zone settings
  float stickDeadZone = 0.15f;
  float triggerDeadZone = 0.01f;

  void reset() {
    for (int i = 0; i < SDL_GAMEPAD_BUTTON_COUNT; ++i) {
      buttonsPressed[i] = false;
      buttonsReleased[i] = false;
    }
  }
};

// Manages all connected gamepads
class RUNA_API GamepadManager {
public:
  GamepadManager();
  ~GamepadManager();

  // Initialize gamepad system
  void initialize();
  void shutdown();

  // Update from SDL events
  void update(const SDL_Event &event);
  void beginFrame();

  // Gamepad queries
  bool isButtonDown(int playerIndex, SDL_GamepadButton button) const;
  bool isButtonPressed(int playerIndex, SDL_GamepadButton button) const;
  bool isButtonReleased(int playerIndex, SDL_GamepadButton button) const;

  float getAxis(int playerIndex, SDL_GamepadAxis axis) const;
  float getLeftStickX(int playerIndex) const;
  float getLeftStickY(int playerIndex) const;
  float getRightStickX(int playerIndex) const;
  float getRightStickY(int playerIndex) const;
  float getLeftTrigger(int playerIndex) const;
  float getRightTrigger(int playerIndex) const;

  // Device management
  bool isGamepadConnected(int playerIndex) const;
  int getConnectedGamepadCount() const;

  // Dead zone configuration
  void setStickDeadZone(int playerIndex, float deadZone);
  void setTriggerDeadZone(int playerIndex, float deadZone);

private:
  std::unordered_map<SDL_JoystickID, GamepadState> m_gamepads;
  int m_nextPlayerIndex = 0;

  GamepadState *getGamepad(int playerIndex);
  const GamepadState *getGamepad(int playerIndex) const;

  void addGamepad(SDL_JoystickID deviceID);
  void removeGamepad(SDL_JoystickID instanceID);

  float applyDeadZone(float value, float deadZone) const;
};

} // namespace Runa

#endif // RUNA_CORE_GAMEPADMANAGER_H
