/**
 * Scene.h
 * Base class for all game scenes (states) in the engine.
 * Provides lifecycle hooks for scene transitions and updates.
 */

#pragma once

#include "../RunaAPI.h"
#include <string>

namespace Runa {

	// Forward declarations
	class Application;
	class Renderer;
	class Input;

	/**
	 * Base class for all scenes.
	 * Scenes represent different game states (menu, gameplay, pause, etc.)
	 */
	class RUNA_API Scene {
	public:
		Scene(Application& app, const std::string& name);
		virtual ~Scene() = default;

		// Lifecycle hooks
		virtual void onEnter() {}      // Called when scene becomes active
		virtual void onExit() {}       // Called when scene is removed
		virtual void onPause() {}      // Called when scene is paused (another scene pushed on top)
		virtual void onResume() {}     // Called when scene resumes (top scene popped)

		virtual void onUpdate(float dt) = 0;  // Update logic
		virtual void onRender() = 0;          // Render scene

		// Scene properties
		const std::string& getName() const { return m_name; }
		bool isTransparent() const { return m_transparent; }
		void setTransparent(bool transparent) { m_transparent = transparent; }

	protected:
		// Access to application systems
		Application& getApp() { return m_app; }
		Renderer& getRenderer();
		Input& getInput();

		std::string m_name;
		bool m_transparent = false;  // If true, scene below will also render

	private:
		Application& m_app;
	};

}  // namespace Runa
