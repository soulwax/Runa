/**
 * SceneSerializer.cpp
 * Implementation of scene serialization to/from YAML.
 */

#include "../runapch.h"
#include "SceneSerializer.h"
#include "Scene.h"
#include "Log.h"
#include "../ECS/Components.h"
#include "../ECS/RPGComponents.h"
#include "../ECS/Registry.h"
#include "../Scenes/TestScene.h"
#include <yaml-cpp/yaml.h>
#include <fstream>
#include <sstream>
#include <entt/entt.hpp>

namespace Runa {

    bool SceneSerializer::saveScene(Scene& scene, const std::string& filePath) {
        try {
            YAML::Emitter out;
            out << YAML::BeginMap;

            // Scene metadata
            out << YAML::Key << "scene" << YAML::Value;
            out << YAML::BeginMap;
            out << YAML::Key << "name" << YAML::Value << scene.getName();
            out << YAML::Key << "type" << YAML::Value << (scene.getType().empty() ? "Scene" : scene.getType());
            out << YAML::EndMap;

            // Serialize ECS registry
            if (auto* registry = scene.getRegistry()) {
                out << YAML::Key << "entities" << YAML::Value;
                serializeRegistry(*registry, out);
            }

            // Scene-specific data
            out << YAML::Key << "scene_data" << YAML::Value;
            out << YAML::BeginMap;

            // Try to serialize TestScene-specific data
            if (auto* testScene = dynamic_cast<TestScene*>(&scene)) {
                // Serialize fence tiles
                out << YAML::Key << "fence_tiles" << YAML::Value;
                out << YAML::BeginSeq;
                // Note: We'll need to expose fence tiles from TestScene
                // For now, we'll serialize what we can
                out << YAML::EndSeq;
            }

            out << YAML::EndMap;

            out << YAML::EndMap;

            // Write to file
            std::ofstream file(filePath);
            if (!file.is_open()) {
                LOG_ERROR("Failed to open file for writing: {}", filePath);
                return false;
            }

            file << out.c_str();
            file.close();

            LOG_INFO("Scene saved successfully to: {}", filePath);
            return true;

        } catch (const YAML::Exception& e) {
            LOG_ERROR("YAML error while saving scene: {}", e.what());
            return false;
        } catch (const std::exception& e) {
            LOG_ERROR("Exception while saving scene: {}", e.what());
            return false;
        }
    }

    bool SceneSerializer::loadScene(Scene& scene, const std::string& filePath) {
        try {
            std::ifstream file(filePath);
            if (!file.is_open()) {
                LOG_ERROR("Failed to open file for reading: {}", filePath);
                return false;
            }

            YAML::Node root = YAML::Load(file);
            file.close();

            if (!root.IsMap()) {
                LOG_ERROR("Invalid scene file format: {}", filePath);
                return false;
            }

            // Load scene metadata
            if (root["scene"]) {
                const auto& sceneNode = root["scene"];
                // Could validate scene name/type here
            }

            // Load entities
            if (root["entities"] && scene.getRegistry()) {
                deserializeRegistry(*scene.getRegistry(), root["entities"], scene);
            }

            // Load scene-specific data
            if (root["scene_data"]) {
                const auto& sceneData = root["scene_data"];
                // Handle scene-specific loading
                if (auto* testScene = dynamic_cast<TestScene*>(&scene)) {
                    // Load fence tiles if present
                    if (sceneData["fence_tiles"]) {
                        // Deserialize fence tiles
                    }
                }
            }

            LOG_INFO("Scene loaded successfully from: {}", filePath);
            return true;

        } catch (const YAML::Exception& e) {
            LOG_ERROR("YAML error while loading scene: {}", e.what());
            return false;
        } catch (const std::exception& e) {
            LOG_ERROR("Exception while loading scene: {}", e.what());
            return false;
        }
    }

    void SceneSerializer::serializeRegistry(ECS::EntityRegistry& registry, YAML::Emitter& out) {
        auto& reg = registry.getRegistry();

        out << YAML::BeginSeq;

        // Serialize all entities with their components
        auto view = reg.view<ECS::Active>();
        for (auto entity : view) {
            out << YAML::BeginMap;

            // Entity ID (for reference, though we'll regenerate on load)
            out << YAML::Key << "id" << YAML::Value << static_cast<uint32_t>(entt::to_integral(entity));

            // Serialize components
            bool hasComponents = false;
            if (reg.all_of<ECS::Position>(entity) || reg.all_of<ECS::Velocity>(entity) ||
                reg.all_of<ECS::Size>(entity) || reg.all_of<ECS::Sprite>(entity) ||
                reg.all_of<ECS::Animation>(entity) || reg.all_of<ECS::PlayerInput>(entity) ||
                reg.all_of<ECS::AABB>(entity) || reg.all_of<ECS::Player>(entity) ||
                reg.all_of<ECS::CameraTarget>(entity)) {
                out << YAML::Key << "components" << YAML::Value;
                out << YAML::BeginMap;
                hasComponents = true;
            }

            if (reg.all_of<ECS::Position>(entity)) {
                const auto& pos = reg.get<ECS::Position>(entity);
                out << YAML::Key << "Position" << YAML::Value;
                out << YAML::BeginMap;
                out << YAML::Key << "x" << YAML::Value << pos.x;
                out << YAML::Key << "y" << YAML::Value << pos.y;
                out << YAML::EndMap;
            }

            if (reg.all_of<ECS::Velocity>(entity)) {
                const auto& vel = reg.get<ECS::Velocity>(entity);
                out << YAML::Key << "Velocity" << YAML::Value;
                out << YAML::BeginMap;
                out << YAML::Key << "x" << YAML::Value << vel.x;
                out << YAML::Key << "y" << YAML::Value << vel.y;
                out << YAML::EndMap;
            }

            if (reg.all_of<ECS::Size>(entity)) {
                const auto& size = reg.get<ECS::Size>(entity);
                out << YAML::Key << "Size" << YAML::Value;
                out << YAML::BeginMap;
                out << YAML::Key << "width" << YAML::Value << size.width;
                out << YAML::Key << "height" << YAML::Value << size.height;
                out << YAML::EndMap;
            }

            if (reg.all_of<ECS::Sprite>(entity)) {
                const auto& sprite = reg.get<ECS::Sprite>(entity);
                out << YAML::Key << "Sprite" << YAML::Value;
                out << YAML::BeginMap;
                // Store sprite sheet path/name instead of pointer
                out << YAML::Key << "spriteName" << YAML::Value << sprite.spriteName;
                out << YAML::Key << "tintR" << YAML::Value << sprite.tintR;
                out << YAML::Key << "tintG" << YAML::Value << sprite.tintG;
                out << YAML::Key << "tintB" << YAML::Value << sprite.tintB;
                out << YAML::Key << "tintA" << YAML::Value << sprite.tintA;
                out << YAML::Key << "flipX" << YAML::Value << sprite.flipX;
                out << YAML::Key << "flipY" << YAML::Value << sprite.flipY;
                out << YAML::EndMap;
            }

            if (reg.all_of<ECS::Animation>(entity)) {
                const auto& anim = reg.get<ECS::Animation>(entity);
                out << YAML::Key << "Animation" << YAML::Value;
                out << YAML::BeginMap;
                out << YAML::Key << "currentFrame" << YAML::Value << anim.currentFrame;
                out << YAML::Key << "animationTime" << YAML::Value << anim.animationTime;
                out << YAML::Key << "frameRate" << YAML::Value << anim.frameRate;
                out << YAML::Key << "loop" << YAML::Value << anim.loop;
                out << YAML::EndMap;
            }

            if (reg.all_of<ECS::PlayerInput>(entity)) {
                const auto& input = reg.get<ECS::PlayerInput>(entity);
                out << YAML::Key << "PlayerInput" << YAML::Value;
                out << YAML::BeginMap;
                out << YAML::Key << "speed" << YAML::Value << input.speed;
                out << YAML::EndMap;
            }

            if (reg.all_of<ECS::AABB>(entity)) {
                const auto& aabb = reg.get<ECS::AABB>(entity);
                out << YAML::Key << "AABB" << YAML::Value;
                out << YAML::BeginMap;
                out << YAML::Key << "offsetX" << YAML::Value << aabb.offsetX;
                out << YAML::Key << "offsetY" << YAML::Value << aabb.offsetY;
                out << YAML::Key << "width" << YAML::Value << aabb.width;
                out << YAML::Key << "height" << YAML::Value << aabb.height;
                out << YAML::EndMap;
            }

            // Tags
            if (reg.all_of<ECS::Player>(entity)) {
                out << YAML::Key << "Player" << YAML::Value << true;
            }

            if (reg.all_of<ECS::CameraTarget>(entity)) {
                const auto& target = reg.get<ECS::CameraTarget>(entity);
                out << YAML::Key << "CameraTarget" << YAML::Value;
                out << YAML::BeginMap;
                out << YAML::Key << "smoothing" << YAML::Value << target.smoothing;
                out << YAML::EndMap;
            }

            if (hasComponents) {
                out << YAML::EndMap; // components
            }

            out << YAML::EndMap; // entity
        }

        out << YAML::EndSeq;
    }

    void SceneSerializer::deserializeRegistry(ECS::EntityRegistry& registry, const YAML::Node& node, Scene& scene) {
        if (!node.IsSequence()) {
            LOG_ERROR("Entities node is not a sequence");
            return;
        }

        auto& reg = registry.getRegistry();

        // Clear existing entities (or preserve if needed)
        // reg.clear(); // Uncomment if you want to clear before loading

        for (const auto& entityNode : node) {
            if (!entityNode.IsMap()) continue;

            auto entity = reg.create();
            reg.emplace<ECS::Active>(entity);

            if (!entityNode["components"]) continue;
            const auto& components = entityNode["components"];

            // Position
            if (components["Position"]) {
                const auto& pos = components["Position"];
                reg.emplace<ECS::Position>(entity,
                    pos["x"].as<float>(0.0f),
                    pos["y"].as<float>(0.0f));
            }

            // Velocity
            if (components["Velocity"]) {
                const auto& vel = components["Velocity"];
                reg.emplace<ECS::Velocity>(entity,
                    vel["x"].as<float>(0.0f),
                    vel["y"].as<float>(0.0f));
            }

            // Size
            if (components["Size"]) {
                const auto& size = components["Size"];
                reg.emplace<ECS::Size>(entity,
                    size["width"].as<float>(16.0f),
                    size["height"].as<float>(16.0f));
            }

            // Sprite
            if (components["Sprite"]) {
                const auto& spriteNode = components["Sprite"];
                ECS::Sprite sprite;
                sprite.spriteName = spriteNode["spriteName"].as<std::string>("");
                sprite.tintR = spriteNode["tintR"].as<float>(1.0f);
                sprite.tintG = spriteNode["tintG"].as<float>(1.0f);
                sprite.tintB = spriteNode["tintB"].as<float>(1.0f);
                sprite.tintA = spriteNode["tintA"].as<float>(1.0f);
                sprite.flipX = spriteNode["flipX"].as<bool>(false);
                sprite.flipY = spriteNode["flipY"].as<bool>(false);
                // Note: spriteSheet pointer will need to be set by the scene after loading
                // The scene should match spriteName to the appropriate sprite sheet
                reg.emplace<ECS::Sprite>(entity, sprite);
            }

            // Animation
            if (components["Animation"]) {
                const auto& animNode = components["Animation"];
                ECS::Animation anim;
                anim.currentFrame = animNode["currentFrame"].as<int>(0);
                anim.animationTime = animNode["animationTime"].as<float>(0.0f);
                anim.frameRate = animNode["frameRate"].as<float>(10.0f);
                anim.loop = animNode["loop"].as<bool>(true);
                reg.emplace<ECS::Animation>(entity, anim);
            }

            // PlayerInput
            if (components["PlayerInput"]) {
                const auto& inputNode = components["PlayerInput"];
                reg.emplace<ECS::PlayerInput>(entity,
                    inputNode["speed"].as<float>(100.0f));
            }

            // AABB
            if (components["AABB"]) {
                const auto& aabbNode = components["AABB"];
                ECS::AABB aabb;
                aabb.offsetX = aabbNode["offsetX"].as<float>(0.0f);
                aabb.offsetY = aabbNode["offsetY"].as<float>(0.0f);
                aabb.width = aabbNode["width"].as<float>(16.0f);
                aabb.height = aabbNode["height"].as<float>(16.0f);
                reg.emplace<ECS::AABB>(entity, aabb);
            }

            // Tags
            if (components["Player"]) {
                reg.emplace<ECS::Player>(entity);
            }

            // CameraTarget
            if (components["CameraTarget"]) {
                const auto& targetNode = components["CameraTarget"];
                reg.emplace<ECS::CameraTarget>(entity,
                    targetNode["smoothing"].as<float>(0.15f));
            }
        }

        LOG_INFO("Deserialized {} entities", node.size());
    }

}
