// File: src/Scenes/TestScene.cpp

/**
 * TestScene.cpp
 * Test scene with meadow, fences, flowers, and player.
 */

#include "../runapch.h"
#include "TestScene.h"
#include "../Core/Application.h"
#include "../Core/Log.h"
#include "../Core/SceneSerializer.h"
#include "../Graphics/PixelScale.h"
#include "../Graphics/Texture.h"
#include "../ECS/Components.h"
#include "../ECS/RPGComponents.h"
#include "../ECS/Systems.h"
#include "../Collision/CollisionLoader.h"
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <SDL3/SDL.h>
#include <filesystem>
#include <set>
#include <unordered_map>

namespace Runa {

	TestScene::TestScene(Application& app)
		: Scene(app, "TestScene") {
		m_type = "TestScene";
	}

	void TestScene::onEnter() {
		LOG_INFO("TestScene: Entering");

		srand(static_cast<unsigned>(time(nullptr)));

		m_spriteBatch = std::make_unique<SpriteBatch>(getRenderer());

		// Initialize camera
		m_camera = std::make_unique<Camera>(getApp().getWindow());
		m_camera->setPosition(0.0f, 0.0f);

		m_font = std::make_unique<Font>(getRenderer(),
		                                "Resources/Fonts/Renogare.ttf", 24);

		if (!m_font->isValid()) {
			LOG_WARN("Failed to load font, FPS display may not work");
		}

		// Load keybindings from metadata file
		m_keybindings = std::make_unique<Keybindings>();
		if (!m_keybindings->loadFromFile("Resources/keybindings.yaml")) {
			LOG_WARN("Failed to load keybindings, using default bindings");
		}

		// Load base grass sprite sheet
		try {
			m_baseGrassSheet = std::make_unique<SpriteSheet>(getRenderer(),
			                                                  "Resources/SpiteSheets/grass.png");

			m_baseGrassSheet->addSprite("grass_base", 0, 0, 16, 16);
			LOG_INFO("Base grass sprite sheet loaded");
		} catch (const std::exception& e) {
			LOG_ERROR("Failed to load base grass sprite sheet: {}", e.what());
		}

		// Load decorative grass sprite sheet
		try {
			m_decorGrassSheet = std::make_unique<SpriteSheet>(getRenderer(),
			                                                  "Resources/SpiteSheets/decor-grass.png");

			// Add decorative grass sprites (16x16 each)
			// First row (0-3)
			m_decorGrassSheet->addSprite("decor_grass_0", 0, 0, 16, 16);
			m_decorGrassSheet->addSprite("decor_grass_1", 16, 0, 16, 16);
			m_decorGrassSheet->addSprite("decor_grass_2", 32, 0, 16, 16);
			m_decorGrassSheet->addSprite("decor_grass_3", 48, 0, 16, 16);
			// Second row (4-7)
			m_decorGrassSheet->addSprite("decor_grass_4", 0, 16, 16, 16);
			m_decorGrassSheet->addSprite("decor_grass_5", 16, 16, 16, 16);
			m_decorGrassSheet->addSprite("decor_grass_6", 32, 16, 16, 16);
			m_decorGrassSheet->addSprite("decor_grass_7", 48, 16, 16, 16);
			// Third row (8-11)
			m_decorGrassSheet->addSprite("decor_grass_8", 0, 32, 16, 16);
			m_decorGrassSheet->addSprite("decor_grass_9", 16, 32, 16, 16);
			m_decorGrassSheet->addSprite("decor_grass_10", 32, 32, 16, 16);
			m_decorGrassSheet->addSprite("decor_grass_11", 48, 32, 16, 16);
			// Fourth row (12-15)
			m_decorGrassSheet->addSprite("decor_grass_12", 0, 48, 16, 16);
			m_decorGrassSheet->addSprite("decor_grass_13", 16, 48, 16, 16);
			m_decorGrassSheet->addSprite("decor_grass_14", 32, 48, 16, 16);
			m_decorGrassSheet->addSprite("decor_grass_15", 48, 48, 16, 16);

			LOG_INFO("Decorative grass sprite sheet loaded");
		} catch (const std::exception& e) {
			LOG_ERROR("Failed to load decorative grass sprite sheet: {}", e.what());
		}

		// Load flowers sprite sheet
		try {
			m_flowersSheet = std::make_unique<SpriteSheet>(getRenderer(),
			                                               "Resources/SpiteSheets/flowers.png");

			// Add flower sprites (16x16 each) based on flowers.yaml
			m_flowersSheet->addSprite("poppy", 0, 0, 16, 16);           // id 0
			m_flowersSheet->addSprite("sunflower", 16, 0, 16, 16);      // id 1
			m_flowersSheet->addSprite("blue_orchid", 32, 0, 16, 16);    // id 2
			m_flowersSheet->addSprite("tulip", 48, 0, 16, 16);          // id 3
			m_flowersSheet->addSprite("allium", 0, 16, 16, 16);         // id 4
			m_flowersSheet->addSprite("daisy", 16, 16, 16, 16);         // id 5
			m_flowersSheet->addSprite("dandelion", 48, 16, 16, 16);      // id 6: atlas_x: 48, atlas_y: 16

			LOG_INFO("Flowers sprite sheet loaded");
		} catch (const std::exception& e) {
			LOG_ERROR("Failed to load flowers sprite sheet: {}", e.what());
		}

		// Load fence sprite sheet
		try {
			m_fenceSheet = std::make_unique<SpriteSheet>(getRenderer(),
			                                             "Resources/SpiteSheets/fences.png");

			// Add fence sprites based on new fences.yaml structure
			// Each sprite is 16x16 (tile_size: 16), despite YAML comments about "1x3" or "3x1" which refer to connection patterns
			// Vertical fences (each is 16x16, positioned at different Y offsets)
			m_fenceSheet->addSprite("vertical_wood_fence_north", 0, 0, 16, 16);      // id 0: atlas_x: 0, atlas_y: 0
			m_fenceSheet->addSprite("vertical_wood_fence_center", 0, 16, 16, 16);    // id 1: atlas_x: 0, atlas_y: 16
			m_fenceSheet->addSprite("vertical_wood_fence_south", 0, 32, 16, 16);     // id 2: atlas_x: 0, atlas_y: 32

			// Horizontal fences (each is 16x16, positioned at different X offsets)
			m_fenceSheet->addSprite("horizontal_wood_fence_west", 16, 48, 16, 16);   // id 13: atlas_x: 16, atlas_y: 48
			m_fenceSheet->addSprite("horizontal_wood_fence_center", 32, 48, 16, 16);  // id 14: atlas_x: 32, atlas_y: 48
			m_fenceSheet->addSprite("horizontal_wood_fence_east", 48, 48, 16, 16);   // id 15: atlas_x: 48, atlas_y: 48

			// Corners (each is 16x16)
			m_fenceSheet->addSprite("wood_fence_corner_northwest", 16, 0, 16, 16);   // id 9: atlas_x: 16, atlas_y: 0
			m_fenceSheet->addSprite("wood_fence_corner_northeast", 32, 0, 16, 16);   // id 10: atlas_x: 32, atlas_y: 0
			m_fenceSheet->addSprite("wood_fence_corner_southeast", 48, 32, 16, 16);  // id 11: atlas_x: 48, atlas_y: 32
			m_fenceSheet->addSprite("wood_fence_corner_southwest", 16, 32, 16, 16);  // id 12: atlas_x: 16, atlas_y: 32

			LOG_INFO("Fence sprite sheet loaded with new YAML structure");
		} catch (const std::exception& e) {
			LOG_ERROR("Failed to load fence sprite sheet: {}", e.what());
		}

		// Load player sprite sheet
		try {
			m_playerSheet = std::make_unique<SpriteSheet>(getRenderer(),
			                                              "Resources/SpiteSheets/characters/player.png");

			// Add idle sprites (first frame of each direction)
			m_playerSheet->addSprite("player_idle_down", 0, 0, 48, 48);
			m_playerSheet->addSprite("player_idle_left", 0, 48, 48, 48);
			m_playerSheet->addSprite("player_idle_right", 0, 48, 48, 48);  // Same as left, will be flipped
			m_playerSheet->addSprite("player_idle_up", 0, 96, 48, 48);

			// Add walking animations (6 frames each, 0.1s per frame, looping)
			// Down: row 0 (y=0), frames at x: 0, 48, 96, 144, 192, 240
			m_playerSheet->addAnimation("player_walk_down", 0, 0, 48, 48, 6, 6, 0.1f, true);
			
			// Left: row 1 (y=48), frames at x: 0, 48, 96, 144, 192, 240
			m_playerSheet->addAnimation("player_walk_left", 0, 48, 48, 48, 6, 6, 0.1f, true);
			
			// Right: same as left (will be flipped horizontally if needed)
			m_playerSheet->addAnimation("player_walk_right", 0, 48, 48, 48, 6, 6, 0.1f, true);
			
			// Up: row 2 (y=96), frames at x: 0, 48, 96, 144, 192, 240
			m_playerSheet->addAnimation("player_walk_up", 0, 96, 48, 48, 6, 6, 0.1f, true);

			LOG_INFO("Player sprite sheet loaded with walking animations");
		} catch (const std::exception& e) {
			LOG_ERROR("Failed to load player sprite sheet: {}", e.what());
		}

		generateMeadow();

		// Initialize ECS registry
		m_registry = std::make_unique<ECS::EntityRegistry>();

		// Create player entity using ECS
		// Player sprite is 48x48 pixels (16x16 logical with 3x scale)
		m_playerEntity = m_registry->createPlayer(0.0f, 0.0f, m_playerSheet.get(), "player_idle_down", 120.0f);

		// Update player size to match sprite (48x48 = 16x16 logical)
		auto& playerSize = m_registry->getRegistry().get<ECS::Size>(m_playerEntity);
		playerSize.width = 16.0f;
		playerSize.height = 16.0f;

		// Update AABB to match player size
		if (auto* aabb = m_registry->getRegistry().try_get<ECS::AABB>(m_playerEntity)) {
			aabb->width = 14.0f;   // Slightly smaller than sprite for better feel
			aabb->height = 14.0f;
			aabb->offsetX = 1.0f;  // Center the collision box
			aabb->offsetY = 1.0f;
		}

		// Ensure Animation component exists with proper settings
		auto& anim = m_registry->getRegistry().get<ECS::Animation>(m_playerEntity);
		anim.frameRate = 10.0f;  // 10 frames per second (0.1s per frame)
		anim.loop = true;
		
		// Add collision component to player
		m_registry->getRegistry().emplace<ECS::Collider>(m_playerEntity);
		auto& playerCollider = m_registry->getRegistry().get<ECS::Collider>(m_playerEntity);
		playerCollider.type = ECS::Collider::Type::Solid;
		playerCollider.blocksMovement = true;
		
		// Add CanInteract component so player can interact with objects
		m_registry->getRegistry().emplace<ECS::CanInteract>(m_playerEntity);
		auto& canInteract = m_registry->getRegistry().get<ECS::CanInteract>(m_playerEntity);
		canInteract.range = 24.0f;  // Interaction range in world units

		LOG_INFO("Player entity created at (0, 0) with collision and interaction");
		
		// Set up collision map for the fence tiles
		setupCollisionMap();

		m_lastFPSUpdate = std::chrono::steady_clock::now();
		m_displayedFPS = 0;
		m_frameCount = 0;

		LOG_INFO("TestScene initialized");
	}

	void TestScene::onExit() {
		LOG_INFO("TestScene: Exiting");
	}

	void TestScene::onPause() {
		LOG_INFO("TestScene: Paused");
	}

	void TestScene::onResume() {
		LOG_INFO("TestScene: Resumed");
	}
	
	bool TestScene::saveScene(const std::string& filePath) {
		// Ensure directory exists
		std::filesystem::path path(filePath);
		std::filesystem::create_directories(path.parent_path());
		
		return SceneSerializer::saveScene(*this, filePath);
	}
	
	bool TestScene::loadScene(const std::string& filePath) {
		// Check if file exists
		if (!std::filesystem::exists(filePath)) {
			LOG_INFO("No saved scene found at: {}, using default scene", filePath);
			return false;
		}
		
		if (!SceneSerializer::loadScene(*this, filePath)) {
			return false;
		}
		
		// After loading, restore sprite sheet pointers based on sprite names
		if (m_registry) {
			auto& registry = m_registry->getRegistry();
			auto spriteView = registry.view<ECS::Sprite>();
			
			for (auto entity : spriteView) {
				auto& sprite = spriteView.get<ECS::Sprite>(entity);
				
				// Match sprite name to appropriate sprite sheet
				if (sprite.spriteName.find("player_") != std::string::npos) {
					sprite.spriteSheet = m_playerSheet.get();
				} else if (sprite.spriteName.find("fence") != std::string::npos || 
				           sprite.spriteName.find("wood_fence") != std::string::npos ||
				           sprite.spriteName.find("vertical_") != std::string::npos ||
				           sprite.spriteName.find("horizontal_") != std::string::npos) {
					sprite.spriteSheet = m_fenceSheet.get();
				} else if (sprite.spriteName.find("decor_grass") != std::string::npos) {
					sprite.spriteSheet = m_decorGrassSheet.get();
				} else if (sprite.spriteName == "grass_base") {
					sprite.spriteSheet = m_baseGrassSheet.get();
				} else if (sprite.spriteName == "poppy" || sprite.spriteName == "sunflower" ||
				           sprite.spriteName == "blue_orchid" || sprite.spriteName == "tulip" ||
				           sprite.spriteName == "allium" || sprite.spriteName == "daisy" ||
				           sprite.spriteName == "dandelion") {
					sprite.spriteSheet = m_flowersSheet.get();
				}
			}
			
			// Find player entity if it exists
			auto playerView = registry.view<ECS::Player>();
			if (!playerView.empty()) {
				m_playerEntity = *playerView.begin();
				LOG_INFO("Player entity restored from save");
			}
		}
		
		return true;
	}

	void TestScene::onUpdate(float deltaTime) {
		// Update ECS systems
		if (m_registry) {
			auto& registry = m_registry->getRegistry();

			// Update player input (handles keyboard input and sets velocity)
			// Pass keybindings so it uses the metadata file
			ECS::Systems::updatePlayerInput(registry, getInput(), deltaTime, m_keybindings.get());

			// Update player animation based on movement direction
			updatePlayerAnimation(registry);
			
			// Apply movement first
			ECS::Systems::updateMovement(registry, deltaTime);
			
			// Then check for collisions and clamp position back if needed
			if (m_collisionMap) {
				ECS::Systems::updateMapCollision(registry, *m_collisionMap, deltaTime,
					[this](entt::entity entity, const ECS::CollisionEvent& event) {
						// Collision detected - log for debugging
						if (entity == m_playerEntity) {
							LOG_DEBUG("Player collision detected!");
						}
					});
			}

			// Update animations (advances frame based on time)
			ECS::Systems::updateAnimation(registry, deltaTime);

			// Update camera to follow player
			ECS::Systems::updateCameraFollow(registry, *m_camera, deltaTime);
			
			// Check for tile interactions (E key)
			if (m_collisionMap) {
				ECS::Systems::updateTileInteraction(registry, *m_collisionMap, getInput(), SDLK_E,
					[this](entt::entity player, TileInteraction& interaction) {
						handleInteraction(player, interaction);
					});
			}
		}

		// Update camera
		m_camera->update(deltaTime);
		
		// Save scene on F5 (for testing)
		if (getInput().isKeyPressed(SDLK_F5)) {
			if (saveScene()) {
				LOG_INFO("Scene saved to Resources/saves/scene_save.yaml");
			}
		}
		
		// Load scene on F6 (for testing)
		if (getInput().isKeyPressed(SDLK_F6)) {
			if (loadScene()) {
				LOG_INFO("Scene loaded from Resources/saves/scene_save.yaml");
			}
		}

		m_frameCount++;

		auto now = std::chrono::steady_clock::now();
		auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
			now - m_lastFPSUpdate).count();

		if (elapsed >= 1000) {
			m_displayedFPS = m_frameCount;
			m_frameCount = 0;
			m_lastFPSUpdate = now;
		}
	}

	void TestScene::onRender() {
		getRenderer().clear(0.2f, 0.4f, 0.6f, 1.0f);

		m_spriteBatch->begin();

		// Get camera bounds for culling
		auto cameraBounds = m_camera->getWorldBounds();
		const int tileSize = 16;
		const int scaledTileSize = PixelScale::getScaledTileSize(tileSize);

		// Render meadow first (so it appears underneath fences)
		// Calculate which tiles are visible (with padding for edge tiles that might be partially visible)
		// Use floor/ceiling to ensure we cover all visible tiles
		// Note: cameraBounds are in world/logical space, so padding should use tileSize (16), not scaledTileSize (48)
		int startTileX = static_cast<int>(std::floor((cameraBounds.left - tileSize) / tileSize));
		int startTileY = static_cast<int>(std::floor((cameraBounds.top - tileSize) / tileSize));
		int endTileX = static_cast<int>(std::ceil((cameraBounds.right + tileSize) / tileSize));
		int endTileY = static_cast<int>(std::ceil((cameraBounds.bottom + tileSize) / tileSize));

		// Render only visible meadow tiles
		if (m_baseGrassSheet) {
			const auto* baseSprite = m_baseGrassSheet->getSprite("grass_base");
			if (baseSprite && !baseSprite->frames.empty()) {
				const auto& baseFrame = baseSprite->frames[0];

				// Meadow extends to a very large area (effectively infinite for gameplay)
				// This ensures meadow is always visible beyond screen edges
				const int worldMeadowSize = 2000;  // 2000x2000 tiles = 32000x32000 pixels
				const int worldMeadowStartX = -(worldMeadowSize * tileSize) >> 1;
				const int worldMeadowStartY = -(worldMeadowSize * tileSize) >> 1;
				const int worldMeadowEndX = worldMeadowStartX + worldMeadowSize * tileSize;
				const int worldMeadowEndY = worldMeadowStartY + worldMeadowSize * tileSize;

				// Clamp to meadow bounds (but meadow is large enough to cover most camera positions)
				int clampedStartX = std::max(startTileX, worldMeadowStartX / tileSize);
				int clampedStartY = std::max(startTileY, worldMeadowStartY / tileSize);
				int clampedEndX = std::min(endTileX, worldMeadowEndX / tileSize);
				int clampedEndY = std::min(endTileY, worldMeadowEndY / tileSize);

				// Render visible tiles (only calculate and render what's actually seen)
				for (int tileY = clampedStartY; tileY < clampedEndY; ++tileY) {
					for (int tileX = clampedStartX; tileX < clampedEndX; ++tileX) {
						// Calculate world position of tile center
						int worldX = tileX * tileSize;
						int worldY = tileY * tileSize;

						// Convert world coordinates to screen coordinates
						int screenX, screenY;
						m_camera->worldToScreen(static_cast<float>(worldX), static_cast<float>(worldY), screenX, screenY);

						// Only render if tile is actually on screen (with some margin for partially visible tiles)
						// This is an additional check to avoid rendering completely off-screen tiles
						if (screenX + scaledTileSize >= 0 && screenX - scaledTileSize < getApp().getWindow().getWidth() &&
							screenY + scaledTileSize >= 0 && screenY - scaledTileSize < getApp().getWindow().getHeight()) {
							// Render base grass
							m_spriteBatch->draw(m_baseGrassSheet->getTexture(),
							                   screenX, screenY,
							                   baseFrame.x, baseFrame.y,
							                   baseFrame.width, baseFrame.height);

							// Add decorative elements (flowers and grass blades) using deterministic randomness
							// Use tile coordinates as seed for consistent placement
							unsigned int seed = static_cast<unsigned int>(tileX * 73856093u + tileY * 19349663u);
							seed = seed ^ (seed >> 16);
							seed = seed * 2246822507u;
							seed = seed ^ (seed >> 13);
							seed = seed * 3266489917u;
							seed = seed ^ (seed >> 16);

							float randValue = static_cast<float>(seed & 0x7FFFFFFF) / 2147483647.0f;

							// 12% chance for a flower
							if (randValue < 0.12f && m_flowersSheet) {
								// Choose random flower type (7 different flowers)
								int flowerIndex = static_cast<int>((randValue * 10000.0f)) % 7;
								std::string flowerNames[] = {"poppy", "sunflower", "blue_orchid", "tulip",
								                             "allium", "daisy", "dandelion"};
								std::string flowerName = flowerNames[flowerIndex];

								const auto* flowerSprite = m_flowersSheet->getSprite(flowerName);
								if (flowerSprite && !flowerSprite->frames.empty()) {
									const auto& flowerFrame = flowerSprite->frames[0];
									m_spriteBatch->draw(m_flowersSheet->getTexture(),
									                   screenX, screenY,
									                   flowerFrame.x, flowerFrame.y,
									                   flowerFrame.width, flowerFrame.height);
								}
							}
							// 35% chance for decorative grass (fills gaps, appears more often than flowers)
							else if (randValue < 0.47f && m_decorGrassSheet) {
								// Choose random grass blade
								int grassIndex = static_cast<int>((randValue * 10000.0f)) % 16;
								std::string grassName = "decor_grass_" + std::to_string(grassIndex);

								const auto* grassSprite = m_decorGrassSheet->getSprite(grassName);
								if (grassSprite && !grassSprite->frames.empty()) {
									const auto& grassFrame = grassSprite->frames[0];
									m_spriteBatch->draw(m_decorGrassSheet->getTexture(),
									                   screenX, screenY,
									                   grassFrame.x, grassFrame.y,
									                   grassFrame.width, grassFrame.height);
								}
							}
						}
					}
				}
			}
		}

		// Render fence (only visible fences)
		if (m_fenceSheet) {
			for (const auto& tile : m_fenceTiles) {
				// Check if fence tile is visible
				float fenceWorldX = static_cast<float>(tile.x);
				float fenceWorldY = static_cast<float>(tile.y);

				const auto* sprite = m_fenceSheet->getSprite(tile.spriteName);
				if (sprite && !sprite->frames.empty()) {
					const auto& frame = sprite->frames[0];

					// Use logical dimensions for culling (cameraBounds are in world/logical space)
					// The sprite dimensions are already in logical pixels (16, 48, etc.)
					float fenceWidth = static_cast<float>(frame.width);
					float fenceHeight = static_cast<float>(frame.height);

					// Cull fence if completely outside camera bounds (all in world/logical space)
					if (fenceWorldX + fenceWidth < cameraBounds.left ||
						fenceWorldX > cameraBounds.right ||
						fenceWorldY + fenceHeight < cameraBounds.top ||
						fenceWorldY > cameraBounds.bottom) {
						continue; // Skip invisible fence
					}

					// Convert world coordinates to screen coordinates
					int screenX, screenY;
					m_camera->worldToScreen(fenceWorldX, fenceWorldY, screenX, screenY);

					m_spriteBatch->draw(m_fenceSheet->getTexture(),
					                   screenX, screenY,
					                   frame.x, frame.y,
					                   frame.width, frame.height);
				}
			}
		}

		// Render all ECS entities (including player) using the ECS render system
		if (m_registry) {
			// Create a white pixel texture for entities without sprites (if needed)
			// For now, we'll just render entities with sprites
			ECS::Systems::renderSprites(m_registry->getRegistry(), *m_spriteBatch, *m_camera, nullptr);
		}

		// Render FPS counter in white (on top of meadow)
		if (m_font && m_font->isValid()) {
			std::string fpsText = "FPS: " + std::to_string(m_displayedFPS);
			SDL_Color white = {255, 255, 255, 255};
			auto fpsTexture = m_font->renderText(fpsText, white);

			if (fpsTexture) {
				// Use 1/3 scale to compensate for global 3x pixel scale
				m_spriteBatch->draw(*fpsTexture, 10, 10, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f / 3.0f, 1.0f / 3.0f);
			}
		}

		m_spriteBatch->end();
	}

	void TestScene::generateMeadow() {
		const int tileSize = 16;
		const int scaledTileSize = PixelScale::getScaledTileSize(tileSize);  // 48 pixels

		// Inner fenced meadow area (smaller, centered)
		// Note: Meadow tiles are generated on-the-fly during rendering based on camera bounds
		// No need to pre-generate all tiles - this saves memory and ensures only visible tiles are calculated
		const int fencedMeadowSize = 80;  // 80x80 tiles
		const int fencedMeadowStartX = -(fencedMeadowSize * tileSize) >> 1;
		const int fencedMeadowStartY = -(fencedMeadowSize * tileSize) >> 1;

		m_fenceTiles.clear();

		if (m_fenceSheet) {
			// Based on new YAML structure:
			// - All fence sprites are 16x16 (tile_size: 16)
			// - Horizontal fences connect horizontally: west -> center -> east
			// - Vertical fences connect vertically: north -> center -> south
			// - Corners connect to edges and fill center

			// For an 80x80 tile meadow, we need 80 fence tiles per edge (one per tile)
			// Top edge: horizontal fences
			for (int i = 0; i < fencedMeadowSize; ++i) {
				int fenceX = fencedMeadowStartX + i * tileSize;
				int fenceY = fencedMeadowStartY - tileSize;  // Position 1 tile above meadow
				std::string spriteName;

				// Determine which horizontal fence sprite to use based on position
				if (i == 0) {
					spriteName = "horizontal_wood_fence_west";  // First tile (connects to corner)
				} else if (i == fencedMeadowSize - 1) {
					spriteName = "horizontal_wood_fence_east";  // Last tile (connects to corner)
				} else {
					spriteName = "horizontal_wood_fence_center";  // Middle tiles
				}

				m_fenceTiles.push_back({fenceX, fenceY, spriteName});
			}

			// Bottom edge: horizontal fences
			for (int i = 0; i < fencedMeadowSize; ++i) {
				int fenceX = fencedMeadowStartX + i * tileSize;
				int fenceY = fencedMeadowStartY + fencedMeadowSize * tileSize;  // Position at bottom edge
				std::string spriteName;

				if (i == 0) {
					spriteName = "horizontal_wood_fence_west";
				} else if (i == fencedMeadowSize - 1) {
					spriteName = "horizontal_wood_fence_east";
				} else {
					spriteName = "horizontal_wood_fence_center";
				}

				m_fenceTiles.push_back({fenceX, fenceY, spriteName});
			}

			// Left edge: vertical fences (skip corners which are handled separately)
			for (int i = 1; i < fencedMeadowSize - 1; ++i) {
				int fenceX = fencedMeadowStartX - tileSize;  // Position 1 tile left of meadow
				int fenceY = fencedMeadowStartY + i * tileSize;
				std::string spriteName;

				// Determine which vertical fence sprite to use based on position
				if (i == 1) {
					spriteName = "vertical_wood_fence_north";  // First segment (top, after corner)
				} else if (i == fencedMeadowSize - 2) {
					spriteName = "vertical_wood_fence_south";  // Last segment (bottom, before corner)
				} else {
					spriteName = "vertical_wood_fence_center";  // Middle segments
				}

				m_fenceTiles.push_back({fenceX, fenceY, spriteName});
			}

			// Right edge: vertical fences (skip corners)
			for (int i = 1; i < fencedMeadowSize - 1; ++i) {
				int fenceX = fencedMeadowStartX + fencedMeadowSize * tileSize;  // Position at right edge
				int fenceY = fencedMeadowStartY + i * tileSize;
				std::string spriteName;

				if (i == 1) {
					spriteName = "vertical_wood_fence_north";
				} else if (i == fencedMeadowSize - 2) {
					spriteName = "vertical_wood_fence_south";
				} else {
					spriteName = "vertical_wood_fence_center";
				}

				m_fenceTiles.push_back({fenceX, fenceY, spriteName});
			}

			// Add corners at the four corners (16x16 sprites)
			m_fenceTiles.push_back({fencedMeadowStartX - tileSize, fencedMeadowStartY - tileSize, "wood_fence_corner_northwest"});
			m_fenceTiles.push_back({fencedMeadowStartX + fencedMeadowSize * tileSize, fencedMeadowStartY - tileSize, "wood_fence_corner_northeast"});
			m_fenceTiles.push_back({fencedMeadowStartX + fencedMeadowSize * tileSize, fencedMeadowStartY + fencedMeadowSize * tileSize, "wood_fence_corner_southeast"});
			m_fenceTiles.push_back({fencedMeadowStartX - tileSize, fencedMeadowStartY + fencedMeadowSize * tileSize, "wood_fence_corner_southwest"});
		}

		LOG_INFO("Generated meadow: {} fence tiles (meadow tiles generated on-the-fly based on camera)",
		         m_fenceTiles.size());
	}

	void TestScene::updatePlayerAnimation(entt::registry& registry) {
		if (!m_registry || m_playerEntity == entt::null || !registry.valid(m_playerEntity)) {
			return;
		}

		// Get player components
		auto* velocity = registry.try_get<ECS::Velocity>(m_playerEntity);
		auto* sprite = registry.try_get<ECS::Sprite>(m_playerEntity);
		auto* position = registry.try_get<ECS::Position>(m_playerEntity);
		
		if (!velocity || !sprite || !sprite->spriteSheet) {
			return;
		}

		// Determine movement direction and speed
		float speed = std::sqrt(velocity->x * velocity->x + velocity->y * velocity->y);
		bool isMoving = speed > 0.1f;  // Threshold to avoid jitter

		std::string newSpriteName;
		bool newFlipX = sprite->flipX;

		if (isMoving) {
			// Determine primary direction (prioritize vertical over horizontal for diagonal movement)
			if (std::abs(velocity->y) > std::abs(velocity->x)) {
				// Vertical movement
				if (velocity->y > 0) {
					newSpriteName = "player_walk_down";
					newFlipX = false;  // No flip for down
				} else {
					newSpriteName = "player_walk_up";
					newFlipX = false;  // No flip for up
				}
			} else {
				// Horizontal movement - use right-facing sprite for both directions
				// Flip horizontally when moving left
				newSpriteName = "player_walk_right";
				newFlipX = (velocity->x < 0);  // Flip when moving left
			}
		} else {
			// Not moving - use idle animation based on last direction
			// If we don't have a last direction, default to down
			if (sprite->spriteName.find("walk_down") != std::string::npos || 
			    sprite->spriteName.find("idle_down") != std::string::npos) {
				newSpriteName = "player_idle_down";
				newFlipX = false;  // No flip for down
			} else if (sprite->spriteName.find("walk_up") != std::string::npos ||
			           sprite->spriteName.find("idle_up") != std::string::npos) {
				newSpriteName = "player_idle_up";
				newFlipX = false;  // No flip for up
			} else {
				// For horizontal directions, use right-facing idle and maintain flip state
				newSpriteName = "player_idle_right";
				// Maintain the flip state from the last movement direction
				// If the sprite name contains "left" or flipX was true, keep it flipped
				if (sprite->spriteName.find("left") != std::string::npos || sprite->flipX) {
					newFlipX = true;   // Keep flipped if was facing left
				} else {
					newFlipX = false;  // No flip if was facing right
				}
			}
		}

		// Check if direction changed (sprite name or flip state)
		bool directionChanged = (sprite->spriteName != newSpriteName) || (sprite->flipX != newFlipX);

		// Only update sprite if it changed
		if (sprite->spriteName != newSpriteName) {
			sprite->spriteName = newSpriteName;
			
			// Reset animation when changing sprites
			if (auto* anim = registry.try_get<ECS::Animation>(m_playerEntity)) {
				anim->animationTime = 0.0f;
				anim->currentFrame = 0;
			}
		}

		// Update flip state
		if (sprite->flipX != newFlipX) {
			sprite->flipX = newFlipX;
		}

		// Log position when direction changes
		if (directionChanged && position) {
			// Determine actual facing direction for clearer logging
			std::string actualDirection;
			if (newSpriteName.find("down") != std::string::npos) {
				actualDirection = "down";
			} else if (newSpriteName.find("up") != std::string::npos) {
				actualDirection = "up";
			} else if (newFlipX) {
				actualDirection = "left";  // Right sprite flipped = facing left
			} else {
				actualDirection = "right";
			}
		LOG_INFO("Player facing '{}' (sprite: '{}', flipX: {}) at position ({}, {})", 
		         actualDirection, newSpriteName, newFlipX, position->x, position->y);
	}
}

void TestScene::setupCollisionMap() {
	// Calculate world size based on meadow
	// Meadow is 80x80 tiles, fences extend beyond, so we need a larger world
	// Fences can be at negative coordinates, so we need to account for that
	int worldSize = MEADOW_SIZE * TILE_SIZE * 3;  // Large enough to cover negative coords
	int worldWidth = worldSize;
	int worldHeight = worldSize;
	
	m_collisionMap = std::make_unique<CollisionMap>(worldWidth, worldHeight, TILE_SIZE);
	
	// Load fence tile definitions from YAML (supports existing format with has_collision, blocks_movement)
	int defsLoaded = CollisionLoader::loadFromYAML("Resources/SpiteSheets/fences.yaml", 
	                                               *m_collisionMap, m_fenceSheet.get());
	LOG_INFO("Loaded {} fence tile definitions for collision", defsLoaded);
	
	// Create pixel-perfect collision masks for fence sprites
	// Get the sprite frame to extract pixel data
	std::unordered_map<std::string, std::shared_ptr<CollisionMask>> fenceMasks;
	
	// Pre-generate masks for each unique fence sprite
	std::set<std::string> uniqueSprites;
	for (const auto& fenceTile : m_fenceTiles) {
		uniqueSprites.insert(fenceTile.spriteName);
	}
	
	for (const std::string& spriteName : uniqueSprites) {
		const auto* sprite = m_fenceSheet->getSprite(spriteName);
		if (sprite && !sprite->frames.empty()) {
			const auto& frame = sprite->frames[0];
			// Create pixel-perfect mask from sprite alpha channel
			auto mask = CollisionLoader::createMaskFromSprite(
				m_fenceSheet.get(), frame.x, frame.y, frame.width, frame.height, 128);
			fenceMasks[spriteName] = mask;
			if (mask && mask->isValid()) {
				LOG_INFO("Created pixel-perfect mask for fence sprite '{}' ({}x{})", 
				         spriteName, mask->getWidth(), mask->getHeight());
			} else {
				LOG_WARN("Failed to create pixel-perfect mask for fence sprite '{}'", spriteName);
			}
		}
	}
	
	// Create tile definitions with pixel-perfect masks
	std::unordered_map<std::string, int> spriteToDefIndex;
	
	for (const std::string& spriteName : uniqueSprites) {
		TileDefinition fenceDef;
		fenceDef.name = "fence_" + spriteName;
		fenceDef.collision = CollisionType::Solid;
		
		// Assign pixel-perfect mask if available
		auto it = fenceMasks.find(spriteName);
		if (it != fenceMasks.end() && it->second && it->second->isValid()) {
			fenceDef.pixelMask = it->second;
			LOG_DEBUG("Assigned pixel-perfect mask to tile definition '{}'", fenceDef.name);
		} else {
			LOG_WARN("No valid pixel mask for sprite '{}', using solid collision", spriteName);
		}
		
		int defIndex = m_collisionMap->addTileDefinition(fenceDef);
		spriteToDefIndex[spriteName] = defIndex;
	}
	
	// Add collision for each placed fence tile with pixel-perfect masks
	int placedCount = 0;
	for (const auto& fenceTile : m_fenceTiles) {
		auto it = spriteToDefIndex.find(fenceTile.spriteName);
		if (it != spriteToDefIndex.end()) {
			m_collisionMap->placeTile(it->second, fenceTile.x, fenceTile.y, TILE_SIZE, TILE_SIZE);
			placedCount++;
		} else {
			// Fallback: create generic solid if sprite not found
			static int genericSolidIndex = -1;
			if (genericSolidIndex < 0) {
				TileDefinition genericDef;
				genericDef.name = "generic_solid";
				genericDef.collision = CollisionType::Solid;
				genericSolidIndex = m_collisionMap->addTileDefinition(genericDef);
			}
			m_collisionMap->placeTile(genericSolidIndex, fenceTile.x, fenceTile.y, TILE_SIZE, TILE_SIZE);
			placedCount++;
		}
	}
	
	// Rebuild spatial grid after placing all tiles
	m_collisionMap->rebuildSpatialGrid();
	
	LOG_INFO("Set up pixel-perfect collision for {} fence tiles ({} unique sprites)", 
	         placedCount, uniqueSprites.size());
	
	// Test collision at various positions around a fence tile
	if (!m_fenceTiles.empty()) {
		const auto& testFence = m_fenceTiles[0];
		
		// Test from center
		float centerX = static_cast<float>(testFence.x + TILE_SIZE / 2);
		float centerY = static_cast<float>(testFence.y + TILE_SIZE / 2);
		CollisionType testCol = m_collisionMap->getCollisionAt(centerX, centerY);
		LOG_INFO("Test collision CENTER at fence ({}, {}): {}", 
		         testFence.x, testFence.y, 
		         testCol == CollisionType::Solid ? "SOLID" : "NONE");
		
		// Test from left (approaching from left)
		float leftX = static_cast<float>(testFence.x - 8);
		float leftY = centerY;
		CollisionType testColLeft = m_collisionMap->checkMovement(leftX, leftY, 14.0f, 14.0f);
		LOG_INFO("Test collision FROM LEFT at fence ({}, {}): {}", 
		         testFence.x, testFence.y, 
		         testColLeft == CollisionType::Solid ? "SOLID" : "NONE");
		
		// Test from right (approaching from right)
		float rightX = static_cast<float>(testFence.x + TILE_SIZE + 8);
		float rightY = centerY;
		CollisionType testColRight = m_collisionMap->checkMovement(rightX, rightY, 14.0f, 14.0f);
		LOG_INFO("Test collision FROM RIGHT at fence ({}, {}): {}", 
		         testFence.x, testFence.y, 
		         testColRight == CollisionType::Solid ? "SOLID" : "NONE");
	}
}

void TestScene::handleInteraction(entt::entity player, TileInteraction& interaction) {
	switch (interaction.type) {
		case InteractionType::Read:
			LOG_INFO("Read interaction: {}", interaction.data);
			// TODO: Display message on screen
			break;
			
		case InteractionType::Container:
			LOG_INFO("Container interaction: loot table '{}'", interaction.data);
			// TODO: Open loot UI
			break;
			
		case InteractionType::Teleport:
			LOG_INFO("Teleport to scene '{}' at ({}, {})", 
			         interaction.targetScene, interaction.targetX, interaction.targetY);
			// TODO: Trigger scene transition
			break;
			
		case InteractionType::Toggle:
			LOG_INFO("Toggle interaction");
			// TODO: Toggle state
			break;
			
		case InteractionType::Pickup:
			LOG_INFO("Pickup interaction: '{}'", interaction.data);
			// TODO: Add to inventory
			break;
			
		case InteractionType::Talk:
			LOG_INFO("Talk interaction: NPC '{}'", interaction.data);
			// TODO: Start dialogue
			break;
			
		default:
			break;
	}
}

}
