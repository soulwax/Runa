/**
 * InputManager.cpp
 * Implementation of the comprehensive input management system.
 */

#include "runapch.h"
#include "InputManager.h"
#include "Log.h"
#include <algorithm>
#include <fstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace Runa {

	InputManager::InputManager(Input& input)
		: m_input(input)
		, m_gamepadManager(std::make_unique<GamepadManager>())
	{
	}

	InputManager::~InputManager() {
		shutdown();
	}

	void InputManager::initialize() {
		LOG_INFO("Initializing InputManager...");
		m_gamepadManager->initialize();


		createContext("Default");
		setActiveContext("Default");

		LOG_INFO("InputManager initialized");
	}

	void InputManager::shutdown() {
		LOG_INFO("Shutting down InputManager...");
		m_gamepadManager->shutdown();
		m_contexts.clear();
		m_activeContexts.clear();
		m_actionCache.clear();
	}

	void InputManager::update(const SDL_Event& event) {

		m_gamepadManager->update(event);
	}

	void InputManager::beginFrame() {

		m_actionCache.clear();


		m_gamepadManager->beginFrame();
	}



	InputContext* InputManager::createContext(const std::string& name) {
		auto it = m_contexts.find(name);
		if (it != m_contexts.end()) {
			return it->second.get();
		}

		auto context = std::make_unique<InputContext>(name);
		context->getBindingSet() = InputBindingSet(name);
		auto* ptr = context.get();
		m_contexts[name] = std::move(context);

		LOG_DEBUG("Created input context: {}", name);
		return ptr;
	}

	InputContext* InputManager::getContext(const std::string& name) {
		auto it = m_contexts.find(name);
		return (it != m_contexts.end()) ? it->second.get() : nullptr;
	}

	void InputManager::pushContext(const std::string& name) {

		if (!m_activeContexts.empty() && m_activeContexts.back() == name) {
			return;
		}


		m_activeContexts.erase(
			std::remove(m_activeContexts.begin(), m_activeContexts.end(), name),
			m_activeContexts.end()
		);

		m_activeContexts.push_back(name);
		LOG_DEBUG("Pushed input context: {}", name);
	}

	void InputManager::popContext() {
		if (m_activeContexts.empty()) {
			LOG_WARN("Cannot pop context: stack is empty");
			return;
		}

		std::string popped = m_activeContexts.back();
		m_activeContexts.pop_back();
		LOG_DEBUG("Popped input context: {}", popped);
	}

	void InputManager::setActiveContext(const std::string& name) {
		m_activeContexts.clear();
		m_activeContexts.push_back(name);
		LOG_DEBUG("Set active input context: {}", name);
	}



	ActionValue InputManager::getAction(const std::string& actionName) const {

		auto cacheIt = m_actionCache.find(actionName);
		if (cacheIt != m_actionCache.end()) {
			return cacheIt->second;
		}

		ActionValue result;


		for (auto it = m_activeContexts.rbegin(); it != m_activeContexts.rend(); ++it) {
			auto contextIt = m_contexts.find(*it);
			if (contextIt == m_contexts.end()) continue;

			const InputContext* context = contextIt->second.get();
			if (!context->isEnabled()) continue;

			const InputBinding* binding = context->getBindingSet().getBinding(actionName);
			if (binding) {
				result = evaluateAction(*binding);
				break;
			}
		}


		m_actionCache[actionName] = result;
		return result;
	}

	bool InputManager::isActionPressed(const std::string& actionName) const {
		return getAction(actionName).isPressed();
	}

	bool InputManager::isActionDown(const std::string& actionName) const {
		return getAction(actionName).isDown();
	}

	bool InputManager::isActionReleased(const std::string& actionName) const {
		return getAction(actionName).isReleased();
	}

	float InputManager::getActionAxis(const std::string& actionName) const {
		return getAction(actionName).getAxis();
	}

	float InputManager::getActionAxisX(const std::string& actionName) const {
		return getAction(actionName).getAxisX();
	}

	float InputManager::getActionAxisY(const std::string& actionName) const {
		return getAction(actionName).getAxisY();
	}



	void InputManager::bindKey(const std::string& contextName, const std::string& actionName, SDL_Keycode key) {
		InputContext* context = createContext(contextName);
		InputBinding* binding = context->getBindingSet().addBinding(actionName, ActionType::Button);
		binding->addSource(InputSource::keyboard(key));
	}

	void InputManager::bindGamepadButton(const std::string& contextName, const std::string& actionName,
	                                      SDL_GamepadButton button, int playerIndex) {
		InputContext* context = createContext(contextName);
		InputBinding* binding = context->getBindingSet().addBinding(actionName, ActionType::Button);
		binding->addSource(InputSource::gamepadButton(button, playerIndex));
	}

	void InputManager::bindAxis(const std::string& contextName, const std::string& actionName,
	                            SDL_GamepadAxis axis, float scale, int playerIndex) {
		InputContext* context = createContext(contextName);
		InputBinding* binding = context->getBindingSet().addBinding(actionName, ActionType::Axis1D);
		binding->addSource(InputSource::gamepadAxis(axis, scale, playerIndex));
	}

	void InputManager::bind2DAxis(const std::string& contextName, const std::string& actionName,
	                               SDL_Keycode up, SDL_Keycode down, SDL_Keycode left, SDL_Keycode right) {
		InputContext* context = createContext(contextName);
		InputBinding* binding = context->getBindingSet().addBinding(actionName, ActionType::Axis2D);


		auto upSrc = InputSource::keyboard(up);
		upSrc.scale = -1.0f;
		binding->addSource(upSrc);

		auto downSrc = InputSource::keyboard(down);
		downSrc.scale = 1.0f;
		binding->addSource(downSrc);

		auto leftSrc = InputSource::keyboard(left);
		leftSrc.scale = -1.0f;
		binding->addSource(leftSrc);

		auto rightSrc = InputSource::keyboard(right);
		rightSrc.scale = 1.0f;
		binding->addSource(rightSrc);
	}




	static std::string inputTypeToString(InputType type) {
		switch (type) {
			case InputType::Keyboard: return "Keyboard";
			case InputType::MouseButton: return "MouseButton";
			case InputType::MouseAxis: return "MouseAxis";
			case InputType::GamepadButton: return "GamepadButton";
			case InputType::GamepadAxis: return "GamepadAxis";
			case InputType::GamepadTrigger: return "GamepadTrigger";
			default: return "Unknown";
		}
	}


	static InputType stringToInputType(const std::string& str) {
		if (str == "Keyboard") return InputType::Keyboard;
		if (str == "MouseButton") return InputType::MouseButton;
		if (str == "MouseAxis") return InputType::MouseAxis;
		if (str == "GamepadButton") return InputType::GamepadButton;
		if (str == "GamepadAxis") return InputType::GamepadAxis;
		if (str == "GamepadTrigger") return InputType::GamepadTrigger;
		return InputType::Keyboard;
	}


	static std::string actionTypeToString(ActionType type) {
		switch (type) {
			case ActionType::Button: return "Button";
			case ActionType::Axis1D: return "Axis1D";
			case ActionType::Axis2D: return "Axis2D";
			default: return "Button";
		}
	}


	static ActionType stringToActionType(const std::string& str) {
		if (str == "Button") return ActionType::Button;
		if (str == "Axis1D") return ActionType::Axis1D;
		if (str == "Axis2D") return ActionType::Axis2D;
		return ActionType::Button;
	}


	static json serializeSource(const InputSource& source) {
		json j;
		j["type"] = inputTypeToString(source.type);
		j["code"] = source.code;
		j["scale"] = source.scale;
		j["deadZone"] = source.deadZone;
		j["deviceIndex"] = source.device.deviceIndex;
		return j;
	}


	static InputSource deserializeSource(const json& j) {
		InputSource source;
		source.type = stringToInputType(j.value("type", "Keyboard"));
		source.code = j.value("code", 0);
		source.scale = j.value("scale", 1.0f);
		source.deadZone = j.value("deadZone", 0.1f);
		source.device.deviceIndex = j.value("deviceIndex", 0);
		return source;
	}

	bool InputManager::saveBindings(const std::string& filePath) const {
		try {
			json root;
			json contextsArray = json::array();


			for (const auto& [name, context] : m_contexts) {
				json contextJson;
				contextJson["name"] = name;
				contextJson["enabled"] = context->isEnabled();

				json bindingsArray = json::array();


				const auto& bindings = context->getBindingSet().getBindings();
				for (const auto& [actionName, binding] : bindings) {
					json bindingJson;
					bindingJson["action"] = actionName;
					bindingJson["type"] = actionTypeToString(binding.getType());

					json sourcesArray = json::array();
					for (const auto& source : binding.getSources()) {
						sourcesArray.push_back(serializeSource(source));
					}
					bindingJson["sources"] = sourcesArray;

					bindingsArray.push_back(bindingJson);
				}

				contextJson["bindings"] = bindingsArray;
				contextsArray.push_back(contextJson);
			}

			root["contexts"] = contextsArray;


			std::ofstream file(filePath);
			if (!file.is_open()) {
				LOG_ERROR("Failed to open file for writing: {}", filePath);
				return false;
			}

			file << root.dump(2);
			file.close();

			LOG_INFO("Saved input bindings to: {}", filePath);
			return true;

		} catch (const std::exception& e) {
			LOG_ERROR("Exception while saving bindings: {}", e.what());
			return false;
		}
	}

	bool InputManager::loadBindings(const std::string& filePath) {
		try {

			std::ifstream file(filePath);
			if (!file.is_open()) {
				LOG_ERROR("Failed to open file for reading: {}", filePath);
				return false;
			}


			json root = json::parse(file);
			file.close();


			auto defaultContext = m_contexts.find("Default");
			if (defaultContext != m_contexts.end()) {
				auto defaultPtr = std::move(defaultContext->second);
				m_contexts.clear();
				m_contexts["Default"] = std::move(defaultPtr);
			} else {
				m_contexts.clear();
			}


			if (root.contains("contexts") && root["contexts"].is_array()) {
				for (const auto& contextJson : root["contexts"]) {
					std::string name = contextJson.value("name", "Unnamed");
					bool enabled = contextJson.value("enabled", true);


					InputContext* context = createContext(name);
					context->setEnabled(enabled);


					if (contextJson.contains("bindings") && contextJson["bindings"].is_array()) {
						for (const auto& bindingJson : contextJson["bindings"]) {
							std::string actionName = bindingJson.value("action", "");
							std::string typeStr = bindingJson.value("type", "Button");
							ActionType type = stringToActionType(typeStr);


							InputBinding* binding = context->getBindingSet().addBinding(actionName, type);


							binding->clearSources();


							if (bindingJson.contains("sources") && bindingJson["sources"].is_array()) {
								for (const auto& sourceJson : bindingJson["sources"]) {
									InputSource source = deserializeSource(sourceJson);
									binding->addSource(source);
								}
							}
						}
					}
				}
			}

			LOG_INFO("Loaded input bindings from: {}", filePath);
			return true;

		} catch (const json::parse_error& e) {
			LOG_ERROR("JSON parse error while loading bindings: {}", e.what());
			return false;
		} catch (const std::exception& e) {
			LOG_ERROR("Exception while loading bindings: {}", e.what());
			return false;
		}
	}


	ActionValue InputManager::evaluateAction(const InputBinding& binding) const {
		ActionValue value;
		value.type = binding.getType();

		const auto& sources = binding.getSources();
		if (sources.empty()) {
			return value;
		}

		switch (binding.getType()) {
			case ActionType::Button: {

				for (const auto& source : sources) {
					if (evaluateSourceButton(source)) {
						value.buttonDown = true;
					}
					if (evaluateSourcePressed(source)) {
						value.buttonPressed = true;
					}
					if (evaluateSourceReleased(source)) {
						value.buttonReleased = true;
					}
				}
				break;
			}

			case ActionType::Axis1D: {

				for (const auto& source : sources) {
					value.axis1D += evaluateSource(source);
				}
				value.axis1D = std::clamp(value.axis1D, -1.0f, 1.0f);
				value.buttonDown = (std::abs(value.axis1D) > 0.01f);
				break;
			}

			case ActionType::Axis2D: {

				for (const auto& source : sources) {
					float val = evaluateSource(source);




					if (source.type == InputType::Keyboard) {
						if (source.scale < 0) {


							if (value.axis2DY == 0.0f) {
								value.axis2DY += val;
							} else {
								value.axis2DX += val;
							}
						} else {

							if (value.axis2DY == 0.0f) {
								value.axis2DY += val;
							} else {
								value.axis2DX += val;
							}
						}
					} else if (source.type == InputType::GamepadAxis) {

						int axisIdx = source.code;
						if (axisIdx == SDL_GAMEPAD_AXIS_LEFTX || axisIdx == SDL_GAMEPAD_AXIS_RIGHTX) {
							value.axis2DX += val;
						} else if (axisIdx == SDL_GAMEPAD_AXIS_LEFTY || axisIdx == SDL_GAMEPAD_AXIS_RIGHTY) {
							value.axis2DY += val;
						}
					}
				}

				value.axis2DX = std::clamp(value.axis2DX, -1.0f, 1.0f);
				value.axis2DY = std::clamp(value.axis2DY, -1.0f, 1.0f);
				value.buttonDown = (std::abs(value.axis2DX) > 0.01f || std::abs(value.axis2DY) > 0.01f);
				break;
			}
		}

		return value;
	}

	float InputManager::evaluateSource(const InputSource& source) const {
		switch (source.type) {
			case InputType::Keyboard: {
				SDL_Keycode key = static_cast<SDL_Keycode>(source.code);
				return m_input.isKeyDown(key) ? source.scale : 0.0f;
			}

			case InputType::MouseButton: {
				return m_input.isMouseButtonDown(source.code) ? source.scale : 0.0f;
			}

			case InputType::GamepadButton: {
				SDL_GamepadButton button = static_cast<SDL_GamepadButton>(source.code);
				return m_gamepadManager->isButtonDown(source.device.deviceIndex, button) ? source.scale : 0.0f;
			}

			case InputType::GamepadAxis: {
				SDL_GamepadAxis axis = static_cast<SDL_GamepadAxis>(source.code);
				float value = m_gamepadManager->getAxis(source.device.deviceIndex, axis);
				return value * source.scale;
			}

			default:
				return 0.0f;
		}
	}

	bool InputManager::evaluateSourceButton(const InputSource& source) const {
		switch (source.type) {
			case InputType::Keyboard:
				return m_input.isKeyDown(static_cast<SDL_Keycode>(source.code));

			case InputType::MouseButton:
				return m_input.isMouseButtonDown(source.code);

			case InputType::GamepadButton:
				return m_gamepadManager->isButtonDown(source.device.deviceIndex,
				                                       static_cast<SDL_GamepadButton>(source.code));

			case InputType::GamepadAxis: {
				float value = m_gamepadManager->getAxis(source.device.deviceIndex,
				                                         static_cast<SDL_GamepadAxis>(source.code));
				return std::abs(value) > source.deadZone;
			}

			default:
				return false;
		}
	}

	bool InputManager::evaluateSourcePressed(const InputSource& source) const {
		switch (source.type) {
			case InputType::Keyboard:
				return m_input.isKeyPressed(static_cast<SDL_Keycode>(source.code));

			case InputType::MouseButton:
				return m_input.isMouseButtonPressed(source.code);

			case InputType::GamepadButton:
				return m_gamepadManager->isButtonPressed(source.device.deviceIndex,
				                                          static_cast<SDL_GamepadButton>(source.code));

			default:
				return false;
		}
	}

	bool InputManager::evaluateSourceReleased(const InputSource& source) const {
		switch (source.type) {
			case InputType::Keyboard:
				return m_input.isKeyReleased(static_cast<SDL_Keycode>(source.code));

			case InputType::MouseButton:
				return m_input.isMouseButtonReleased(source.code);

			case InputType::GamepadButton:
				return m_gamepadManager->isButtonReleased(source.device.deviceIndex,
				                                           static_cast<SDL_GamepadButton>(source.code));

			default:
				return false;
		}
	}

}
