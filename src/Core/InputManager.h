/**
 * InputManager.h
 * Comprehensive input management system with action mapping,
 * input contexts, and multi-device support.
 */

#pragma once

#include "../RunaAPI.h"
#include "Input.h"
#include "GamepadManager.h"
#include "InputAction.h"
#include "InputBinding.h"
#include <memory>
#include <unordered_map>
#include <string>
#include <vector>

namespace Runa {

	// Input context - represents a set of bindings for a specific game state
	class RUNA_API InputContext {
	public:
		InputContext(const std::string& name)
			: m_name(name), m_enabled(true), m_bindingSet(name) {}

		const std::string& getName() const { return m_name; }
		bool isEnabled() const { return m_enabled; }
		void setEnabled(bool enabled) { m_enabled = enabled; }

		// Get or create binding set
		InputBindingSet& getBindingSet() { return m_bindingSet; }
		const InputBindingSet& getBindingSet() const { return m_bindingSet; }

	private:
		std::string m_name;
		bool m_enabled;
		InputBindingSet m_bindingSet;
	};

	// Main input manager
	class RUNA_API InputManager {
	public:
		InputManager(Input& input);
		~InputManager();

		// Initialize/shutdown
		void initialize();
		void shutdown();

		// Frame update (call at beginning of frame)
		void update(const SDL_Event& event);
		void beginFrame();

		// ===== Context Management =====

		// Create or get a context
		InputContext* createContext(const std::string& name);
		InputContext* getContext(const std::string& name);

		// Push/pop contexts (stack-based)
		void pushContext(const std::string& name);
		void popContext();
		void setActiveContext(const std::string& name);  // Replace all with one context

		// Get currently active contexts
		const std::vector<std::string>& getActiveContexts() const { return m_activeContexts; }

		// ===== Action Queries =====

		// Get action value from active contexts (searches from top to bottom)
		ActionValue getAction(const std::string& actionName) const;

		// Quick accessors
		bool isActionPressed(const std::string& actionName) const;
		bool isActionDown(const std::string& actionName) const;
		bool isActionReleased(const std::string& actionName) const;
		float getActionAxis(const std::string& actionName) const;
		float getActionAxisX(const std::string& actionName) const;
		float getActionAxisY(const std::string& actionName) const;

		// ===== Binding Helpers =====

		// Quick bind functions for common cases
		void bindKey(const std::string& contextName, const std::string& actionName, SDL_Keycode key);
		void bindGamepadButton(const std::string& contextName, const std::string& actionName, SDL_GamepadButton button, int playerIndex = 0);
		void bindAxis(const std::string& contextName, const std::string& actionName, SDL_GamepadAxis axis, float scale = 1.0f, int playerIndex = 0);

		// Composite bindings
		void bind2DAxis(const std::string& contextName, const std::string& actionName,
		                SDL_Keycode up, SDL_Keycode down, SDL_Keycode left, SDL_Keycode right);

		// ===== Device Access =====

		Input& getRawInput() { return m_input; }
		GamepadManager& getGamepadManager() { return *m_gamepadManager; }

		// ===== Serialization =====

		// Save/load bindings to JSON
		bool saveBindings(const std::string& filePath) const;
		bool loadBindings(const std::string& filePath);

	private:
		Input& m_input;
		std::unique_ptr<GamepadManager> m_gamepadManager;

		// Contexts
		std::unordered_map<std::string, std::unique_ptr<InputContext>> m_contexts;
		std::vector<std::string> m_activeContexts;  // Stack of active contexts

		// Action value cache (updated each frame)
		mutable std::unordered_map<std::string, ActionValue> m_actionCache;

		// Helper functions
		ActionValue evaluateAction(const InputBinding& binding) const;
		float evaluateSource(const InputSource& source) const;
		bool evaluateSourceButton(const InputSource& source) const;
		bool evaluateSourcePressed(const InputSource& source) const;
		bool evaluateSourceReleased(const InputSource& source) const;
	};

}  // namespace Runa
