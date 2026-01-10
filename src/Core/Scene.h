// File: src/Core/Scene.h

/**
 * Scene.h
 * Base class for all game scenes (states) in the engine.
 * Provides lifecycle hooks for scene transitions and updates.
 */

#ifndef RUNA_CORE_SCENE_H
#define RUNA_CORE_SCENE_H

#include "../RunaAPI.h"
#include <string>

namespace Runa {


	class Application;
	class Renderer;
	class Input;
	
	// Forward declarations for ECS
	namespace ECS {
		class EntityRegistry;
	}

	/**
	 * Base class for all scenes.
	 * Scenes represent different game states (menu, gameplay, pause, etc.)
	 */
	class RUNA_API Scene {
	public:
		Scene(Application& app, const std::string& name);
		virtual ~Scene() = default;


		virtual void onEnter() {}
		virtual void onExit() {}
		virtual void onPause() {}
		virtual void onResume() {}

		virtual void onUpdate(float dt) = 0;
		virtual void onRender() = 0;


		const std::string& getName() const { return m_name; }
		std::string getType() const { return m_type; }
		bool isTransparent() const { return m_transparent; }
		void setTransparent(bool transparent) { m_transparent = transparent; }
		
		// For serialization
		virtual ECS::EntityRegistry* getRegistry() { return nullptr; }

	protected:

		Application& getApp() { return m_app; }
		Renderer& getRenderer();
		Input& getInput();

		std::string m_name;
		std::string m_type;
		bool m_transparent = false;

	private:
		Application& m_app;
	};

}

#endif
