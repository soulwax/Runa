// File: src/Core/ResourceManager.cpp

#include "ResourceManager.h"
#include <yaml-cpp/yaml.h>
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <filesystem>

namespace Runa {

ResourceManager::ResourceManager(Renderer& renderer)
    : m_renderer(renderer) {
    std::cout << "ResourceManager initialized" << std::endl;
}

void ResourceManager::loadSpriteSheetFromYAML(const std::string& yamlPath) {
    std::cout << "Loading spritesheet manifest: " << yamlPath << std::endl;

    try {
        YAML::Node config = YAML::LoadFile(yamlPath);

        if (!config["spritesheet"]) {
            throw std::runtime_error("YAML file missing 'spritesheet' root node");
        }

        YAML::Node sheetNode = config["spritesheet"];

        // Get spritesheet name
        std::string name = sheetNode["name"].as<std::string>();

        // Get texture path (resolve relative to YAML file location)
        std::string texturePath = sheetNode["texture"].as<std::string>();
        std::filesystem::path yamlDir = std::filesystem::path(yamlPath).parent_path();
        std::string fullTexturePath = (yamlDir / texturePath).string();

        // Create spritesheet
        auto spriteSheet = std::make_unique<SpriteSheet>(m_renderer, fullTexturePath);

        // Parse sprites
        if (sheetNode["sprites"]) {
            for (const auto& spriteNode : sheetNode["sprites"]) {
                std::string spriteName = spriteNode["name"].as<std::string>();
                std::string type = spriteNode["type"] ? spriteNode["type"].as<std::string>() : "single";

                if (type == "single") {
                    // Single sprite frame
                    int x = spriteNode["x"].as<int>();
                    int y = spriteNode["y"].as<int>();
                    int width = spriteNode["width"].as<int>();
                    int height = spriteNode["height"].as<int>();

                    spriteSheet->addSprite(spriteName, x, y, width, height);

                } else if (type == "animation") {
                    // Animation with multiple frames
                    int x = spriteNode["x"].as<int>();
                    int y = spriteNode["y"].as<int>();
                    int frameWidth = spriteNode["frame_width"].as<int>();
                    int frameHeight = spriteNode["frame_height"].as<int>();
                    int frameCount = spriteNode["frame_count"].as<int>();
                    int columns = spriteNode["columns"] ? spriteNode["columns"].as<int>() : frameCount;
                    float frameDuration = spriteNode["frame_duration"] ? spriteNode["frame_duration"].as<float>() : 0.1f;
                    bool loop = spriteNode["loop"] ? spriteNode["loop"].as<bool>() : true;

                    spriteSheet->addAnimation(spriteName, x, y, frameWidth, frameHeight,
                                              frameCount, columns, frameDuration, loop);

                } else if (type == "grid") {
                    // Grid-based tileset
                    int tileWidth = spriteNode["tile_width"].as<int>();
                    int tileHeight = spriteNode["tile_height"].as<int>();
                    int columns = spriteNode["columns"] ? spriteNode["columns"].as<int>() : 0;
                    int rows = spriteNode["rows"] ? spriteNode["rows"].as<int>() : 0;

                    spriteSheet->createGrid(spriteName, tileWidth, tileHeight, columns, rows);

                } else if (type == "frames") {
                    // Manually specified frames
                    std::vector<SpriteFrame> frames;
                    bool loop = spriteNode["loop"] ? spriteNode["loop"].as<bool>() : true;

                    for (const auto& frameNode : spriteNode["frame_list"]) {
                        SpriteFrame frame;
                        frame.x = frameNode["x"].as<int>();
                        frame.y = frameNode["y"].as<int>();
                        frame.width = frameNode["width"].as<int>();
                        frame.height = frameNode["height"].as<int>();
                        frame.duration = frameNode["duration"] ? frameNode["duration"].as<float>() : 0.1f;
                        frames.push_back(frame);
                    }

                    spriteSheet->addSpriteFrames(spriteName, frames, loop);
                }
            }
        }

        // Store spritesheet
        m_spriteSheets[name] = std::move(spriteSheet);
        std::cout << "Loaded spritesheet '" << name << "' with "
                  << m_spriteSheets[name]->getSpriteNames().size() << " sprites/animations" << std::endl;

    } catch (const YAML::Exception& e) {
        throw std::runtime_error("YAML parsing error in " + yamlPath + ": " + e.what());
    } catch (const std::exception& e) {
        throw std::runtime_error("Error loading spritesheet from " + yamlPath + ": " + e.what());
    }
}

SpriteSheet* ResourceManager::getSpriteSheet(const std::string& name) {
    auto it = m_spriteSheets.find(name);
    return it != m_spriteSheets.end() ? it->second.get() : nullptr;
}

const SpriteSheet* ResourceManager::getSpriteSheet(const std::string& name) const {
    auto it = m_spriteSheets.find(name);
    return it != m_spriteSheets.end() ? it->second.get() : nullptr;
}

const Sprite* ResourceManager::getSprite(const std::string& sheetName, const std::string& spriteName) const {
    const SpriteSheet* sheet = getSpriteSheet(sheetName);
    return sheet ? sheet->getSprite(spriteName) : nullptr;
}

std::vector<std::string> ResourceManager::getSpriteSheetNames() const {
    std::vector<std::string> names;
    names.reserve(m_spriteSheets.size());
    for (const auto& pair : m_spriteSheets) {
        names.push_back(pair.first);
    }
    return names;
}

void ResourceManager::clear() {
    m_spriteSheets.clear();
    std::cout << "ResourceManager cleared" << std::endl;
}

} // namespace Runa
