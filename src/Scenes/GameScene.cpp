/**
 * GameScene.cpp
 * Implementation of the main gameplay scene.
 */

#include "../runapch.h"
#include "GameScene.h"
#include "PauseScene.h"
#include "../Core/Application.h"
#include "../Core/SceneManager.h"
#include "../Core/Log.h"
#include "../Graphics/Renderer.h"
#include "../Graphics/Window.h"
#include "../ECS/Components.h"
#include "../ECS/RPGComponents.h"
#include "../ECS/Systems.h"
#include "../ECS/RPGComponents.h"

namespace Runa {

	GameScene::GameScene(Application& app)
		: Scene(app, "GameScene") {
	}

	void GameScene::onEnter() {
		LOG_INFO("GameScene: Entering");

		m_spriteBatch = std::make_unique<SpriteBatch>(getRenderer());
		m_grassTexture = std::make_unique<Texture>(getRenderer(), "Resources/SpiteSheets/grass.png");
		m_camera = std::make_unique<Camera>(getApp().getWindow());
		m_registry = std::make_unique<ECS::EntityRegistry>();


		m_tileMap = std::make_unique<TileMap>(40, 30, TILE_SIZE);
		for (int y = 0; y < 30; ++y) {
			for (int x = 0; x < 40; ++x) {
				m_tileMap->setTile(x, y, 0);
			}
		}


		m_player = m_registry->createEntity(320.0f, 240.0f);
		auto& sprite = m_registry->getRegistry().emplace<ECS::Sprite>(m_player);
		sprite.spriteSheet = nullptr;
		sprite.spriteName = "";
		sprite.tintR = 1.0f;
		sprite.tintG = 0.4f;
		sprite.tintB = 0.4f;
		sprite.tintA = 1.0f;
		m_registry->getRegistry().emplace<ECS::Size>(m_player, 16.0f, 16.0f);
		m_registry->getRegistry().emplace<ECS::PlayerInput>(m_player, 150.0f);
		m_registry->getRegistry().emplace<ECS::Player>(m_player);
		m_registry->getRegistry().emplace<ECS::CameraTarget>(m_player);

		LOG_INFO("GameScene: Player created at (320, 240)");
	}

	void GameScene::onExit() {
		LOG_INFO("GameScene: Exiting");
	}

	void GameScene::onPause() {
		LOG_INFO("GameScene: Paused");
	}

	void GameScene::onResume() {
		LOG_INFO("GameScene: Resumed");
	}

	void GameScene::onUpdate(float dt) {

		if (getInput().isKeyPressed(SDLK_ESCAPE)) {
			LOG_INFO("Opening pause menu...");
			getApp().getSceneManager().pushScene(std::make_unique<PauseScene>(getApp()));
			return;
		}


		ECS::Systems::updatePlayerInput(m_registry->getRegistry(), getInput(), dt);
		ECS::Systems::updateMovement(m_registry->getRegistry(), dt);


		auto& pos = m_registry->getRegistry().get<ECS::Position>(m_player);
		m_camera->setPosition(pos.x, pos.y);
		m_camera->update(dt);
	}

	void GameScene::onRender() {
		getRenderer().clear(0.1f, 0.1f, 0.15f, 1.0f);

		m_spriteBatch->begin();


		auto bounds = m_camera->getWorldBounds();
		int startX = std::max(0, static_cast<int>(bounds.left / TILE_SIZE));
		int startY = std::max(0, static_cast<int>(bounds.top / TILE_SIZE));
		int endX = std::min(39, static_cast<int>(bounds.right / TILE_SIZE));
		int endY = std::min(29, static_cast<int>(bounds.bottom / TILE_SIZE));

		for (int y = startY; y <= endY; ++y) {
			for (int x = startX; x <= endX; ++x) {
				int screenX, screenY;
				m_camera->worldToScreen(x * TILE_SIZE, y * TILE_SIZE, screenX, screenY);
				m_spriteBatch->draw(*m_grassTexture, screenX, screenY, 0, 0, 
				                    static_cast<int>(TILE_SIZE), static_cast<int>(TILE_SIZE));
			}
		}


		ECS::Systems::renderSprites(m_registry->getRegistry(), *m_spriteBatch, *m_camera);

		m_spriteBatch->end();
	}

}
