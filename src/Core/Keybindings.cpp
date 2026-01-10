/**
 * Keybindings.cpp
 * Implementation of keybindings loader.
 */

#include "../runapch.h"
#include "Keybindings.h"
#include "Log.h"
#include <fstream>
#include <yaml-cpp/yaml.h>

namespace Runa {

	std::vector<SDL_Keycode> Keybindings::s_emptyVector;

	Keybindings::Keybindings() {
	}

	bool Keybindings::loadFromFile(const std::string& filePath) {
		try {
			YAML::Node root = YAML::LoadFile(filePath);

			if (!root.IsMap()) {
				LOG_ERROR("Keybindings file is not a valid YAML map: {}", filePath);
				return false;
			}

			m_bindings.clear();

			for (const auto& node : root) {
				std::string actionName = node.first.as<std::string>();
				YAML::Node keysNode = node.second;

				std::vector<SDL_Keycode> keys;

				if (keysNode.IsSequence()) {
					for (const auto& keyNode : keysNode) {
						if (keyNode.IsScalar()) {
							// Try to parse as integer (SDL_Keycode)
							try {
								SDL_Keycode keycode = static_cast<SDL_Keycode>(keyNode.as<int>());
								keys.push_back(keycode);
							} catch (const YAML::BadConversion& e) {
								LOG_WARN("Failed to parse keycode for action '{}': {}", actionName, e.what());
							}
						}
					}
				}

				if (!keys.empty()) {
					m_bindings[actionName] = keys;
					LOG_DEBUG("Loaded {} keybindings for action '{}'", keys.size(), actionName);
				} else {
					LOG_WARN("No valid keybindings found for action '{}'", actionName);
				}
			}

			LOG_INFO("Loaded keybindings from: {} ({} actions)", filePath, m_bindings.size());
			return true;

		} catch (const YAML::Exception& e) {
			LOG_ERROR("YAML error while loading keybindings from {}: {}", filePath, e.what());
			return false;
		} catch (const std::exception& e) {
			LOG_ERROR("Exception while loading keybindings from {}: {}", filePath, e.what());
			return false;
		}
	}

	const std::vector<SDL_Keycode>& Keybindings::getKeys(const std::string& actionName) const {
		auto it = m_bindings.find(actionName);
		if (it != m_bindings.end()) {
			return it->second;
		}
		return s_emptyVector;
	}

	bool Keybindings::isKeyBound(const std::string& actionName, SDL_Keycode key) const {
		const auto& keys = getKeys(actionName);
		return std::find(keys.begin(), keys.end(), key) != keys.end();
	}

	std::vector<std::string> Keybindings::getActionNames() const {
		std::vector<std::string> names;
		names.reserve(m_bindings.size());
		for (const auto& pair : m_bindings) {
			names.push_back(pair.first);
		}
		return names;
	}

	bool Keybindings::hasAction(const std::string& actionName) const {
		return m_bindings.find(actionName) != m_bindings.end();
	}

}
