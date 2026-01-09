/**
 * GameScene.h
 * Main gameplay scene with ECS-based player movement.
 */

#ifndef RUNA_SCENES_GAMESCENE_H
#define RUNA_SCENES_GAMESCENE_H

#include "../Core/Scene.h"
#include "../Graphics/SpriteBatch.h"
#include "../Graphics/Texture.h"
#include "../Graphics/TileMap.h"
#include "../Graphics/Camera.h"
#include "../ECS/Registry.h"
#include <memory>
#include <entt/entt.hpp>

namespace Runa {

	class RUNA_API GameScene : public Scene {
	public:
		GameScene(Application& app);
		~GameScene() override = default;

		void onEnter() override;
		void onExit() override;
		void onPause() override;
		void onResume() override;
		void onUpdate(float dt) override;
		void onRender() override;

	private:
		std::unique_ptr<SpriteBatch> m_spriteBatch;
		std::unique_ptr<Texture> m_grassTexture;
		std::unique_ptr<TileMap> m_tileMap;
		std::unique_ptr<Camera> m_camera;
		std::unique_ptr<ECS::EntityRegistry> m_registry;

		entt::entity m_player;
		const int TILE_SIZE = 16;
	};

}

#endif
