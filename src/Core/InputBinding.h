/**
 * InputBinding.h
 * Defines how inputs are bound to actions.
 */

#ifndef RUNA_CORE_INPUTBINDING_H
#define RUNA_CORE_INPUTBINDING_H

#include "../RunaAPI.h"
#include "InputAction.h"
#include <vector>
#include <string>
#include <unordered_map>

namespace Runa {

	// Binding for a single action
	class RUNA_API InputBinding {
	public:
		InputBinding(const std::string& name, ActionType type)
			: m_name(name), m_type(type) {}

		// Add an input source to this binding
		void addSource(const InputSource& source) {
			m_sources.push_back(source);
		}

		// Remove a specific source
		void removeSource(size_t index) {
			if (index < m_sources.size()) {
				m_sources.erase(m_sources.begin() + index);
			}
		}

		// Clear all sources
		void clearSources() {
			m_sources.clear();
		}

		// Getters
		const std::string& getName() const { return m_name; }
		ActionType getType() const { return m_type; }
		const std::vector<InputSource>& getSources() const { return m_sources; }
		std::vector<InputSource>& getSources() { return m_sources; }

	private:
		std::string m_name;
		ActionType m_type;
		std::vector<InputSource> m_sources;
	};

	// Collection of bindings (represents a control scheme)
	class RUNA_API InputBindingSet {
	public:
		InputBindingSet(const std::string& name) : m_name(name) {}

		// Create or get a binding
		InputBinding* addBinding(const std::string& actionName, ActionType type) {
			auto it = m_bindings.find(actionName);
			if (it != m_bindings.end()) {
				return &it->second;
			}
			m_bindings.emplace(actionName, InputBinding(actionName, type));
			return &m_bindings.at(actionName);
		}

		// Get a binding by name
		InputBinding* getBinding(const std::string& actionName) {
			auto it = m_bindings.find(actionName);
			return (it != m_bindings.end()) ? &it->second : nullptr;
		}

		const InputBinding* getBinding(const std::string& actionName) const {
			auto it = m_bindings.find(actionName);
			return (it != m_bindings.end()) ? &it->second : nullptr;
		}

		// Remove a binding
		void removeBinding(const std::string& actionName) {
			m_bindings.erase(actionName);
		}

		// Get all bindings
		const std::unordered_map<std::string, InputBinding>& getBindings() const {
			return m_bindings;
		}

		const std::string& getName() const { return m_name; }

	private:
		std::string m_name;
		std::unordered_map<std::string, InputBinding> m_bindings;
	};

}  // namespace Runa

#endif // RUNA_CORE_INPUTBINDING_H
