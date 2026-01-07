/**
 * RPGSystems.cpp
 * Implementation of RPG gameplay systems
 */

#include "runapch.h"
#include "RPGSystems.h"
#include "Components.h"
#include "../Core/Log.h"
#include <cmath>
#include <algorithm>

namespace Runa {
namespace ECS {
namespace RPGSystems {

// Helper: Calculate distance between two positions
static float distance(float x1, float y1, float x2, float y2) {
	float dx = x2 - x1;
	float dy = y2 - y1;
	return std::sqrt(dx * dx + dy * dy);
}

// ===== Combat System =====

void updateCombat(entt::registry& registry, float dt, float gameTime) {
	// Find player
	auto playerView = registry.view<Player, Position, Combat, Health>();
	if (playerView.size_hint() == 0) return;

	auto playerEntity = playerView.front();
	auto& playerPos = playerView.get<Position>(playerEntity);
	auto& playerCombat = playerView.get<Combat>(playerEntity);
	auto& playerHealth = playerView.get<Health>(playerEntity);

	// Player attacks enemies
	auto enemyView = registry.view<Enemy, Position, Health, AABB>();
	for (auto enemyEntity : enemyView) {
		auto& enemyPos = enemyView.get<Position>(enemyEntity);
		auto& enemyHealth = enemyView.get<Health>(enemyEntity);
		auto& enemyAABB = enemyView.get<AABB>(enemyEntity);

		float dist = distance(playerPos.x, playerPos.y, enemyPos.x, enemyPos.y);

		// Check if player can attack
		if (dist <= playerCombat.attackRange && playerCombat.canAttack(gameTime)) {
			enemyHealth.damage(playerCombat.damage);
			playerCombat.lastAttackTime = gameTime;

			// Create damage number
			auto damageNum = registry.create();
			registry.emplace<Position>(damageNum, enemyPos.x, enemyPos.y);
			registry.emplace<DamageNumber>(damageNum, playerCombat.damage, 1.0f, 0.0f, 0.0f, false);

			LOG_DEBUG("Player dealt {} damage to enemy", playerCombat.damage);

			// If enemy dies, give XP to player
			if (enemyHealth.isDead) {
				if (registry.all_of<Experience>(playerEntity)) {
					auto& playerXP = registry.get<Experience>(playerEntity);
					playerXP.addXP(25); // 25 XP per enemy
					LOG_INFO("Enemy defeated! +25 XP");
				}

				// Update quest progress
				auto questView = registry.view<QuestGiver>();
				for (auto questEntity : questView) {
					auto& questGiver = questView.get<QuestGiver>(questEntity);
					if (questGiver.quest.status == QuestStatus::InProgress) {
						questGiver.quest.enemiesKilled++;
					}
				}
			}
		}
	}

	// Enemies attack player
	for (auto enemyEntity : enemyView) {
		auto& enemyPos = enemyView.get<Position>(enemyEntity);
		auto& enemyHealth = enemyView.get<Health>(enemyEntity);

		if (enemyHealth.isDead) continue;

		// Check if enemy has combat component
		if (!registry.all_of<Combat>(enemyEntity)) continue;

		auto& enemyCombat = registry.get<Combat>(enemyEntity);
		float dist = distance(playerPos.x, playerPos.y, enemyPos.x, enemyPos.y);

		if (dist <= enemyCombat.attackRange && enemyCombat.canAttack(gameTime)) {
			playerHealth.damage(enemyCombat.damage);
			enemyCombat.lastAttackTime = gameTime;

			// Create damage number
			auto damageNum = registry.create();
			registry.emplace<Position>(damageNum, playerPos.x, playerPos.y);
			registry.emplace<DamageNumber>(damageNum, enemyCombat.damage, 1.0f, 0.0f, 0.0f, false);

			LOG_DEBUG("Enemy dealt {} damage to player", enemyCombat.damage);

			if (playerHealth.isDead) {
				LOG_INFO("Player has been defeated!");
			}
		}
	}

	// Clean up dead enemies
	for (auto enemyEntity : enemyView) {
		auto& enemyHealth = enemyView.get<Health>(enemyEntity);
		if (enemyHealth.isDead) {
			registry.destroy(enemyEntity);
		}
	}
}

// ===== AI System =====

void updateAI(entt::registry& registry, float dt) {
	// Find player position
	auto playerView = registry.view<Player, Position>();
	if (playerView.size_hint() == 0) return;

	auto playerEntity = playerView.front();
	auto& playerPos = playerView.get<Position>(playerEntity);

	// Update enemy AI
	auto aiView = registry.view<Enemy, Position, Velocity, AIController, Health>();
	for (auto entity : aiView) {
		auto& pos = aiView.get<Position>(entity);
		auto& vel = aiView.get<Velocity>(entity);
		auto& ai = aiView.get<AIController>(entity);
		auto& health = aiView.get<Health>(entity);

		if (health.isDead) {
			ai.state = AIState::Dead;
			vel.x = 0;
			vel.y = 0;
			continue;
		}

		float distToPlayer = distance(pos.x, pos.y, playerPos.x, playerPos.y);

		switch (ai.state) {
			case AIState::Idle: {
				vel.x = 0;
				vel.y = 0;

				// Check if player is in detection range
				if (distToPlayer <= ai.detectionRange) {
					ai.state = AIState::Chase;
					LOG_DEBUG("Enemy detected player!");
				}

				ai.idleTime += dt;
				if (ai.idleTime > 3.0f) {
					ai.state = AIState::Patrol;
					ai.idleTime = 0.0f;
				}
				break;
			}

			case AIState::Patrol: {
				// Generate patrol point if we don't have one
				if (!ai.hasPatrolPoint) {
					ai.patrolX = pos.x + (rand() % 200 - 100);
					ai.patrolY = pos.y + (rand() % 200 - 100);
					ai.hasPatrolPoint = true;
				}

				// Move towards patrol point
				float dx = ai.patrolX - pos.x;
				float dy = ai.patrolY - pos.y;
				float dist = std::sqrt(dx * dx + dy * dy);

				if (dist > 5.0f) {
					vel.x = (dx / dist) * ai.moveSpeed * 0.5f; // Slower patrol
					vel.y = (dy / dist) * ai.moveSpeed * 0.5f;
				} else {
					// Reached patrol point
					ai.hasPatrolPoint = false;
					ai.state = AIState::Idle;
				}

				// Check for player
				if (distToPlayer <= ai.detectionRange) {
					ai.state = AIState::Chase;
				}
				break;
			}

			case AIState::Chase: {
				// Move towards player
				float dx = playerPos.x - pos.x;
				float dy = playerPos.y - pos.y;
				float dist = std::sqrt(dx * dx + dy * dy);

				if (dist > ai.attackRange) {
					vel.x = (dx / dist) * ai.moveSpeed;
					vel.y = (dy / dist) * ai.moveSpeed;
				} else {
					// In attack range
					ai.state = AIState::Attack;
					vel.x = 0;
					vel.y = 0;
				}

				// Lose interest if player is too far
				if (distToPlayer > ai.detectionRange * 1.5f) {
					ai.state = AIState::Idle;
					ai.chaseTime = 0.0f;
				}
				break;
			}

			case AIState::Attack: {
				vel.x = 0;
				vel.y = 0;

				// Combat system handles actual attacking
				// Check if player moved away
				if (distToPlayer > ai.attackRange * 1.5f) {
					ai.state = AIState::Chase;
				}
				break;
			}

			case AIState::Dead: {
				vel.x = 0;
				vel.y = 0;
				break;
			}

			default:
				break;
		}
	}
}

// ===== Item Collection System =====

void updateItemCollection(entt::registry& registry) {
	// Find player
	auto playerView = registry.view<Player, Position, Inventory>();
	if (playerView.size_hint() == 0) return;

	auto playerEntity = playerView.front();
	auto& playerPos = playerView.get<Position>(playerEntity);
	auto& playerInv = playerView.get<Inventory>(playerEntity);

	// Check for nearby items
	auto itemView = registry.view<ItemEntity, Position, DroppedItem>();
	for (auto itemEntity : itemView) {
		auto& itemPos = itemView.get<Position>(itemEntity);
		auto& droppedItem = itemView.get<DroppedItem>(itemEntity);

		if (droppedItem.collected) continue;

		float dist = distance(playerPos.x, playerPos.y, itemPos.x, itemPos.y);
		if (dist <= 32.0f) {
			// Collect item
			if (playerInv.addItem(droppedItem.item)) {
				LOG_INFO("Collected: {}", droppedItem.item.name);
				droppedItem.collected = true;
				registry.destroy(itemEntity);
			} else {
				LOG_WARN("Inventory full!");
			}
		}
	}
}

// ===== Quest System =====

void updateQuests(entt::registry& registry) {
	auto questView = registry.view<QuestGiver>();
	for (auto entity : questView) {
		auto& questGiver = questView.get<QuestGiver>(entity);

		if (questGiver.quest.status == QuestStatus::InProgress) {
			if (questGiver.quest.isComplete() && !questGiver.questCompleted) {
				questGiver.quest.status = QuestStatus::Completed;
				questGiver.questCompleted = true;
				LOG_INFO("Quest completed: {}", questGiver.quest.title);

				// Give rewards to player
				auto playerView = registry.view<Player, Experience, Inventory>();
				if (playerView.size_hint() != 0) {
					auto playerEntity = playerView.front();
					auto& playerXP = playerView.get<Experience>(playerEntity);
					auto& playerInv = playerView.get<Inventory>(playerEntity);

					playerXP.addXP(questGiver.quest.xpReward);
					playerInv.gold += questGiver.quest.goldReward;
					LOG_INFO("Rewards: +{} XP, +{} Gold", questGiver.quest.xpReward, questGiver.quest.goldReward);
				}
			}
		}
	}
}

// ===== Damage Numbers System =====

void updateDamageNumbers(entt::registry& registry, float dt) {
	auto view = registry.view<DamageNumber, Position>();
	for (auto entity : view) {
		auto& dmgNum = view.get<DamageNumber>(entity);
		dmgNum.elapsed += dt;
		dmgNum.offsetY -= 30.0f * dt; // Float upwards

		if (dmgNum.elapsed >= dmgNum.lifetime) {
			registry.destroy(entity);
		}
	}
}

void renderDamageNumbers(entt::registry& registry, SpriteBatch& batch, Font& font, Camera& camera) {
	auto view = registry.view<DamageNumber, Position>();
	for (auto entity : view) {
		auto& dmgNum = view.get<DamageNumber>(entity);
		auto& pos = view.get<Position>(entity);

		int screenX, screenY;
		camera.worldToScreen(pos.x, pos.y + dmgNum.offsetY, screenX, screenY);

		// Fade out over lifetime
		float alpha = 1.0f - (dmgNum.elapsed / dmgNum.lifetime);
		SDL_Color color = dmgNum.isCritical ?
			SDL_Color{255, 50, 50, static_cast<Uint8>(alpha * 255)} :
			SDL_Color{255, 255, 255, static_cast<Uint8>(alpha * 255)};

		std::string text = std::to_string(static_cast<int>(dmgNum.damage));
		auto texture = font.renderText(text, color);
		if (texture) {
			batch.draw(*texture, screenX, screenY);
		}
	}
}

// ===== UI Rendering =====

void renderHealthBars(entt::registry& registry, SpriteBatch& batch, Camera& camera) {
	auto view = registry.view<Position, Health, AABB>();
	for (auto entity : view) {
		auto& pos = view.get<Position>(entity);
		auto& health = view.get<Health>(entity);
		auto& aabb = view.get<AABB>(entity);

		if (health.isDead) continue;

		// Don't show full health bars
		if (health.current >= health.max) continue;

		int screenX, screenY;
		camera.worldToScreen(pos.x, pos.y - 8, screenX, screenY);

		// Draw health bar background (red)
		// We'll use the font system to create colored rectangles
		// For now, just draw text indicating health percentage
	}
}

void renderPlayerUI(entt::registry& registry, SpriteBatch& batch, Font& font, int screenWidth, int screenHeight) {
	auto playerView = registry.view<Player, Health, Experience, Inventory>();
	if (playerView.size_hint() == 0) return;

	auto playerEntity = playerView.front();
	auto& health = playerView.get<Health>(playerEntity);
	auto& xp = playerView.get<Experience>(playerEntity);
	auto& inv = playerView.get<Inventory>(playerEntity);

	int y = 10;

	// Health
	std::string healthText = "HP: " + std::to_string(static_cast<int>(health.current)) +
	                         "/" + std::to_string(static_cast<int>(health.max));
	auto healthTexture = font.renderText(healthText, {255, 50, 50, 255});
	if (healthTexture) {
		batch.draw(*healthTexture, 10, y);
		y += 30;
	}

	// Level and XP
	std::string levelText = "Level: " + std::to_string(xp.level);
	auto levelTexture = font.renderText(levelText, {100, 200, 255, 255});
	if (levelTexture) {
		batch.draw(*levelTexture, 10, y);
		y += 30;
	}

	std::string xpText = "XP: " + std::to_string(xp.currentXP) + "/" + std::to_string(xp.xpToNextLevel);
	auto xpTexture = font.renderText(xpText, {255, 255, 100, 255});
	if (xpTexture) {
		batch.draw(*xpTexture, 10, y);
		y += 30;
	}

	// Gold
	std::string goldText = "Gold: " + std::to_string(inv.gold);
	auto goldTexture = font.renderText(goldText, {255, 215, 0, 255});
	if (goldTexture) {
		batch.draw(*goldTexture, 10, y);
		y += 30;
	}

	// Item count
	std::string itemText = "Items: " + std::to_string(inv.items.size()) + "/" + std::to_string(inv.maxSlots);
	auto itemTexture = font.renderText(itemText, {200, 200, 200, 255});
	if (itemTexture) {
		batch.draw(*itemTexture, 10, y);
	}
}

} // namespace RPGSystems
} // namespace ECS
} // namespace Runa
