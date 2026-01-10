// File: src/Core/InputManager.h

/**
 * InputManager.h
 * Comprehensive input management system with action mapping,
 * input contexts, and multi-device support.
 */

#ifndef RUNA_CORE_INPUTMANAGER_H
#define RUNA_CORE_INPUTMANAGER_H

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


	class RUNA_API InputContext {
	public:
		InputContext(const std::string& name)
			: m_name(name), m_enabled(true), m_bindingSet(name) {}

		const std::string& getName() const { return m_name; }
		bool isEnabled() const { return m_enabled; }
		void setEnabled(bool enabled) { m_enabled = enabled; }


		InputBindingSet& getBindingSet() { return m_bindingSet; }
		const InputBindingSet& getBindingSet() const { return m_bindingSet; }

	private:
		std::string m_name;
		bool m_enabled;
		InputBindingSet m_bindingSet;
	};


	class RUNA_API InputManager {
	public:
		InputManager(Input& input);
		~InputManager();


		void initialize();
		void shutdown();


		void update(const SDL_Event& event);
		void beginFrame();




		InputContext* createContext(const std::string& name);
		InputContext* getContext(const std::string& name);


		void pushContext(const std::string& name);
		void popContext();
		void setActiveContext(const std::string& name);


		const std::vector<std::string>& getActiveContexts() const { return m_activeContexts; }




		ActionValue getAction(const std::string& actionName) const;


		bool isActionPressed(const std::string& actionName) const;
		bool isActionDown(const std::string& actionName) const;
		bool isActionReleased(const std::string& actionName) const;
		float getActionAxis(const std::string& actionName) const;
		float getActionAxisX(const std::string& actionName) const;
		float getActionAxisY(const std::string& actionName) const;




		void bindKey(const std::string& contextName, const std::string& actionName, SDL_Keycode key);
		void bindGamepadButton(const std::string& contextName, const std::string& actionName, SDL_GamepadButton button, int playerIndex = 0);
		void bindAxis(const std::string& contextName, const std::string& actionName, SDL_GamepadAxis axis, float scale = 1.0f, int playerIndex = 0);


		void bind2DAxis(const std::string& contextName, const std::string& actionName,
		                SDL_Keycode up, SDL_Keycode down, SDL_Keycode left, SDL_Keycode right);



		Input& getRawInput() { return m_input; }
		GamepadManager& getGamepadManager() { return *m_gamepadManager; }




		bool saveBindings(const std::string& filePath) const;
		bool loadBindings(const std::string& filePath);

	private:
		Input& m_input;
		std::unique_ptr<GamepadManager> m_gamepadManager;


		std::unordered_map<std::string, std::unique_ptr<InputContext>> m_contexts;
		std::vector<std::string> m_activeContexts;


		mutable std::unordered_map<std::string, ActionValue> m_actionCache;


		ActionValue evaluateAction(const InputBinding& binding) const;
		float evaluateSource(const InputSource& source) const;
		bool evaluateSourceButton(const InputSource& source) const;
		bool evaluateSourcePressed(const InputSource& source) const;
		bool evaluateSourceReleased(const InputSource& source) const;
	};

}

#endif
