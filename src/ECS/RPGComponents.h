// File: src/ECS/RPGComponents.h

/**
 * RPGComponents.h
 * Components specific to RPG gameplay
 */

#ifndef RUNA_ECS_RPGCOMPONENTS_H
#define RUNA_ECS_RPGCOMPONENTS_H

#include "../RunaAPI.h"
#include <string>
#include <vector>
#include <functional>

namespace Runa {
    namespace ECS {



    struct RUNA_API Health {
        float current = 100.0f;
        float max = 100.0f;
        bool isDead = false;

        float getPercent() const { return max > 0 ? current / max : 0.0f; }
        void heal(float amount) { current = std::min(current + amount, max); }
        void damage(float amount) {
            current -= amount;
            if (current <= 0.0f) {
                current = 0.0f;
                isDead = true;
            }
        }
    };

    struct RUNA_API Combat {
        float damage = 10.0f;
        float attackRange = 32.0f;
        float attackCooldown = 1.0f;
        float lastAttackTime = 0.0f;

        bool canAttack(float currentTime) const {
            return (currentTime - lastAttackTime) >= attackCooldown;
        }
    };

    struct RUNA_API Experience {
        int currentXP = 0;
        int level = 1;
        int xpToNextLevel = 100;

        void addXP(int amount) {
            currentXP += amount;
            while (currentXP >= xpToNextLevel) {
                currentXP -= xpToNextLevel;
                level++;
                xpToNextLevel = static_cast<int>(xpToNextLevel * 1.5f);
            }
        }
    };



    enum class AIState {
        Idle,
        Patrol,
        Chase,
        Attack,
        Flee,
        Dead
    };

    struct RUNA_API AIController {
        AIState state = AIState::Idle;
        float detectionRange = 200.0f;
        float attackRange = 32.0f;
        float moveSpeed = 80.0f;
        float chaseTime = 0.0f;
        float idleTime = 0.0f;


        float patrolX = 0.0f;
        float patrolY = 0.0f;
        float patrolRadius = 100.0f;
        bool hasPatrolPoint = false;
    };



    enum class ItemType {
        Potion,
        Coin,
        Key,
        Weapon,
        Armor,
        QuestItem
    };

    struct RUNA_API Item {
        ItemType type;
        std::string name;
        std::string description;
        int value = 0;
        int stackSize = 1;


        float healAmount = 0.0f;
        float damageBonus = 0.0f;

        bool isConsumable() const {
            return type == ItemType::Potion;
        }
    };

    struct RUNA_API DroppedItem {
        Item item;
        bool collected = false;
    };

    struct RUNA_API Inventory {
        std::vector<Item> items;
        int maxSlots = 20;
        int gold = 0;

        bool addItem(const Item& item) {

            if (item.type == ItemType::Coin) {
                gold += item.value;
                return true;
            }


            for (auto& existingItem : items) {
                if (existingItem.name == item.name && existingItem.stackSize < 99) {
                    existingItem.stackSize += item.stackSize;
                    return true;
                }
            }


            if (items.size() < static_cast<size_t>(maxSlots)) {
                items.push_back(item);
                return true;
            }

            return false;
        }

        bool hasItem(const std::string& itemName) const {
            for (const auto& item : items) {
                if (item.name == itemName) return true;
            }
            return false;
        }

        bool removeItem(const std::string& itemName) {
            for (auto it = items.begin(); it != items.end(); ++it) {
                if (it->name == itemName) {
                    items.erase(it);
                    return true;
                }
            }
            return false;
        }
    };



    enum class QuestStatus {
        NotStarted,
        InProgress,
        Completed,
        Failed
    };

    struct RUNA_API Quest {
        std::string id;
        std::string title;
        std::string description;
        QuestStatus status = QuestStatus::NotStarted;


        int enemiesKilled = 0;
        int enemiesRequired = 0;
        std::vector<std::string> itemsRequired;
        std::vector<std::string> itemsCollected;


        int xpReward = 100;
        int goldReward = 50;
        std::vector<Item> itemRewards;

        bool isComplete() const {
            if (status == QuestStatus::Completed) return true;
            if (enemiesRequired > 0 && enemiesKilled < enemiesRequired) return false;
            if (!itemsRequired.empty() && itemsCollected.size() < itemsRequired.size()) return false;
            return status == QuestStatus::InProgress;
        }
    };

    struct RUNA_API QuestGiver {
        std::string npcName;
        std::string dialogueText;
        Quest quest;
        bool questGiven = false;
        bool questCompleted = false;
    };


    // Note: Interactable moved to Components.h with expanded functionality

    struct RUNA_API DamageNumber {
        float damage;
        float lifetime = 1.0f;
        float elapsed = 0.0f;
        float offsetY = 0.0f;
        bool isCritical = false;
    };



    struct RUNA_API Player {};
    struct RUNA_API Enemy {};
    struct RUNA_API NPC {};
    struct RUNA_API ItemEntity {};

    }
}

#endif
