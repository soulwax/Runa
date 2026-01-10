/**
 * Keybindings.h
 * Utility class for loading and accessing keybindings from a metadata file.
 */

#ifndef RUNA_CORE_KEYBINDINGS_H
#define RUNA_CORE_KEYBINDINGS_H

#include "../RunaAPI.h"
#include <SDL3/SDL.h>
#include <string>
#include <vector>
#include <unordered_map>

namespace Runa {

	/**
	 * Keybindings - Loads and manages keybindings from a YAML metadata file.
	 * Provides easy access to key codes for actions.
	 */
	class RUNA_API Keybindings {
	public:
		Keybindings();
		~Keybindings() = default;

		/**
		 * Load keybindings from a YAML file.
		 * @param filePath Path to the keybindings YAML file
		 * @return true if loaded successfully, false otherwise
		 */
		bool loadFromFile(const std::string& filePath);

		/**
		 * Get all key codes for a given action.
		 * @param actionName Name of the action (e.g., "move_up", "pause")
		 * @return Vector of SDL_Keycode values for this action
		 */
		const std::vector<SDL_Keycode>& getKeys(const std::string& actionName) const;

		/**
		 * Check if a key is bound to an action.
		 * @param actionName Name of the action
		 * @param key SDL_Keycode to check
		 * @return true if the key is bound to this action
		 */
		bool isKeyBound(const std::string& actionName, SDL_Keycode key) const;

		/**
		 * Get all available action names.
		 * @return Vector of all action names
		 */
		std::vector<std::string> getActionNames() const;

		/**
		 * Check if an action exists.
		 * @param actionName Name of the action
		 * @return true if the action exists
		 */
		bool hasAction(const std::string& actionName) const;

	private:
		std::unordered_map<std::string, std::vector<SDL_Keycode>> m_bindings;
		static std::vector<SDL_Keycode> s_emptyVector;  // Returned when action not found
	};

}

#endif // RUNA_CORE_KEYBINDINGS_H
