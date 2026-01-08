/**
 * SceneManager.h
 * Manages a stack of scenes with transitions and lifecycle management.
 */

#ifndef RUNA_CORE_SCENEMANAGER_H
#define RUNA_CORE_SCENEMANAGER_H

#include "../RunaAPI.h"
#include "Scene.h"
#include <memory>
#include <vector>
#include <string>

namespace Runa {

	/**
	 * Scene manager with stack-based scene management.
	 * Supports pushing/popping scenes for overlays (pause menus, etc.)
	 */
	class RUNA_API SceneManager {
	public:
		SceneManager() = default;
		~SceneManager() = default;

		// Scene management
		void pushScene(std::unique_ptr<Scene> scene);
		void popScene();
		void changeScene(std::unique_ptr<Scene> scene);  // Replace current scene
		void clearScenes();

		// Update and render
		void update(float dt);
		void render();

		// Queries
		Scene* getCurrentScene() const;
		bool hasScenes() const { return !m_scenes.empty(); }
		size_t getSceneCount() const { return m_scenes.size(); }

	private:
		std::vector<std::unique_ptr<Scene>> m_scenes;
	};

}  // namespace Runa

#endif // RUNA_CORE_SCENEMANAGER_H
