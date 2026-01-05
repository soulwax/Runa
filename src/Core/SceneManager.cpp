/**
 * SceneManager.cpp
 * Implementation of the SceneManager class.
 */

#include "runapch.h"
#include "SceneManager.h"
#include "Log.h"

namespace Runa {

	void SceneManager::pushScene(std::unique_ptr<Scene> scene) {
		if (!scene) {
			LOG_ERROR("Cannot push null scene");
			return;
		}

		// Pause current scene if exists
		if (!m_scenes.empty()) {
			m_scenes.back()->onPause();
		}

		LOG_INFO("Pushing scene: {}", scene->getName());
		m_scenes.push_back(std::move(scene));
		m_scenes.back()->onEnter();
	}

	void SceneManager::popScene() {
		if (m_scenes.empty()) {
			LOG_WARN("Cannot pop scene: stack is empty");
			return;
		}

		LOG_INFO("Popping scene: {}", m_scenes.back()->getName());
		m_scenes.back()->onExit();
		m_scenes.pop_back();

		// Resume previous scene if exists
		if (!m_scenes.empty()) {
			m_scenes.back()->onResume();
		}
	}

	void SceneManager::changeScene(std::unique_ptr<Scene> scene) {
		if (!scene) {
			LOG_ERROR("Cannot change to null scene");
			return;
		}

		// Exit current scene
		if (!m_scenes.empty()) {
			LOG_INFO("Changing scene from {} to {}", m_scenes.back()->getName(), scene->getName());
			m_scenes.back()->onExit();
			m_scenes.pop_back();
		} else {
			LOG_INFO("Changing to scene: {}", scene->getName());
		}

		// Enter new scene
		m_scenes.push_back(std::move(scene));
		m_scenes.back()->onEnter();
	}

	void SceneManager::clearScenes() {
		LOG_INFO("Clearing all scenes");
		while (!m_scenes.empty()) {
			m_scenes.back()->onExit();
			m_scenes.pop_back();
		}
	}

	void SceneManager::update(float dt) {
		if (m_scenes.empty()) return;

		// Only update the top scene (unless we want to update transparent scenes too)
		m_scenes.back()->onUpdate(dt);
	}

	void SceneManager::render() {
		if (m_scenes.empty()) return;

		// Render from bottom to top for transparent scenes
		size_t startIndex = m_scenes.size() - 1;

		// Find first non-transparent scene from the top
		for (int i = static_cast<int>(m_scenes.size()) - 1; i >= 0; --i) {
			if (!m_scenes[i]->isTransparent()) {
				startIndex = i;
				break;
			}
		}

		// Render all scenes from first non-transparent to top
		for (size_t i = startIndex; i < m_scenes.size(); ++i) {
			m_scenes[i]->onRender();
		}
	}

	Scene* SceneManager::getCurrentScene() const {
		return m_scenes.empty() ? nullptr : m_scenes.back().get();
	}

}  // namespace Runa
