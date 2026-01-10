// File: src/Scenes/TestScene.h

/**
 * TestScene.h
 * Test scene with meadow, fences, flowers, and player.
 */

#ifndef RUNA_SCENES_TESTSCENE_H
#define RUNA_SCENES_TESTSCENE_H

#include "../Core/Scene.h"
#include "../Core/Keybindings.h"
#include "../Graphics/SpriteBatch.h"
#include "../Graphics/SpriteSheet.h"
#include "../Graphics/Camera.h"
#include "../Graphics/Font.h"
#include "../ECS/Registry.h"
#include "../Collision/CollisionMap.h"
#include <memory>
#include <entt/entt.hpp>
#include <vector>
#include <string>
#include <chrono>

namespace Runa {

	class RUNA_API TestScene : public Scene {
	public:
		TestScene(Application& app);
		~TestScene() override = default;

		void onEnter() override;
		void onExit() override;
		void onPause() override;
		void onResume() override;
		void onUpdate(float dt) override;
		void onRender() override;
		
		// Serialization support
		ECS::EntityRegistry* getRegistry() override { return m_registry.get(); }
		
		// Save/Load scene state
		bool saveScene(const std::string& filePath = "Resources/saves/scene_save.yaml");
		bool loadScene(const std::string& filePath = "Resources/saves/scene_save.yaml");

	private:
		void generateMeadow();
		void updatePlayerAnimation(entt::registry& registry);
		void setupCollisionMap();
		void handleInteraction(entt::entity player, TileInteraction& interaction);

		std::unique_ptr<SpriteBatch> m_spriteBatch;
		std::unique_ptr<Camera> m_camera;
		std::unique_ptr<Font> m_font;
		std::unique_ptr<Keybindings> m_keybindings;
		
		// Sprite sheets
		std::unique_ptr<SpriteSheet> m_baseGrassSheet;
		std::unique_ptr<SpriteSheet> m_decorGrassSheet;
		std::unique_ptr<SpriteSheet> m_flowersSheet;
		std::unique_ptr<SpriteSheet> m_fenceSheet;
		std::unique_ptr<SpriteSheet> m_playerSheet;

		// ECS registry
		std::unique_ptr<ECS::EntityRegistry> m_registry;
		entt::entity m_playerEntity = entt::null;
		
		// Collision system
		std::unique_ptr<CollisionMap> m_collisionMap;

		// Fence tiles (pre-generated)
		struct FenceTile {
			int x, y;
			std::string spriteName;
		};
		std::vector<FenceTile> m_fenceTiles;

		// FPS counter
		std::chrono::steady_clock::time_point m_lastFPSUpdate;
		int m_displayedFPS = 0;
		int m_frameCount = 0;
		
		// Meadow bounds
		static constexpr int MEADOW_SIZE = 40;  // tiles
		static constexpr int TILE_SIZE = 16;    // pixels
	};

}

#endif // RUNA_SCENES_TESTSCENE_H
