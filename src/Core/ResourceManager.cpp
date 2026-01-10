// File: src/Core/ResourceManager.cpp

#include "../runapch.h"
#include "ResourceManager.h"
#include "Log.h"
#include <yaml-cpp/yaml.h>

namespace Runa {

namespace {

    std::filesystem::path findProjectRoot() {



        const char* basePath = SDL_GetBasePath();
        std::filesystem::path currentPath = basePath ? basePath : std::filesystem::current_path();



        std::filesystem::path checkPath = currentPath;
        for (int i = 0; i < 10; ++i) {
            if (std::filesystem::exists(checkPath / "Resources") ||
                std::filesystem::exists(checkPath / "CMakeLists.txt")) {
                return checkPath;
            }
            if (checkPath.has_parent_path() && checkPath != checkPath.parent_path()) {
                checkPath = checkPath.parent_path();
            } else {
                break;
            }
        }


        return std::filesystem::current_path();
    }
}

ResourceManager::ResourceManager(Renderer& renderer)
    : m_renderer(renderer) {
    LOG_INFO("ResourceManager initialized");
}

void ResourceManager::loadSpriteSheetFromYAML(const std::string& yamlPath) {
    LOG_INFO("Loading spritesheet manifest: {}", yamlPath);

    try {

        std::filesystem::path projectRoot = findProjectRoot();
        std::filesystem::path path(yamlPath);

        if (path.is_relative()) {

            path = projectRoot / path;


            if (!std::filesystem::exists(path)) {
                path = std::filesystem::absolute(yamlPath);
            }
        }

        if (!std::filesystem::exists(path)) {
            throw std::runtime_error("YAML file does not exist: " + path.string());
        }

        LOG_DEBUG("Loading YAML from: {}", path.string());
        YAML::Node config = YAML::LoadFile(path.string());

        if (!config["spritesheet"]) {
            throw std::runtime_error("YAML file missing 'spritesheet' root node");
        }

        YAML::Node sheetNode = config["spritesheet"];


        std::string name = sheetNode["name"].as<std::string>();


        std::string texturePath = sheetNode["texture"].as<std::string>();
        std::filesystem::path yamlDir = path.parent_path();
        std::filesystem::path texturePathObj(texturePath);


        if (texturePathObj.is_relative()) {
            std::string fullTexturePath = (yamlDir / texturePath).string();

            if (!std::filesystem::exists(fullTexturePath)) {
                fullTexturePath = (projectRoot / texturePath).string();
            }
            texturePath = fullTexturePath;
        } else {
            texturePath = texturePathObj.string();
        }


        auto spriteSheet = std::make_unique<SpriteSheet>(m_renderer, texturePath);


        if (sheetNode["sprites"]) {
            for (const auto& spriteNode : sheetNode["sprites"]) {
                std::string spriteName = spriteNode["name"].as<std::string>();
                std::string type = spriteNode["type"] ? spriteNode["type"].as<std::string>() : "single";

                if (type == "single") {

                    int x = spriteNode["x"].as<int>();
                    int y = spriteNode["y"].as<int>();
                    int width = spriteNode["width"].as<int>();
                    int height = spriteNode["height"].as<int>();

                    spriteSheet->addSprite(spriteName, x, y, width, height);

                } else if (type == "animation") {

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

                    int tileWidth = spriteNode["tile_width"].as<int>();
                    int tileHeight = spriteNode["tile_height"].as<int>();
                    int columns = spriteNode["columns"] ? spriteNode["columns"].as<int>() : 0;
                    int rows = spriteNode["rows"] ? spriteNode["rows"].as<int>() : 0;

                    spriteSheet->createGrid(spriteName, tileWidth, tileHeight, columns, rows);

                } else if (type == "frames") {

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


        m_spriteSheets[name] = std::move(spriteSheet);
        LOG_INFO("Loaded spritesheet '{}' with {} sprites/animations",
                 name, m_spriteSheets[name]->getSpriteNames().size());

    } catch (const YAML::Exception& e) {
        throw std::runtime_error("YAML parsing error in " + yamlPath + ": " + e.what());
    } catch (const std::exception& e) {
        throw std::runtime_error("Error loading spritesheet from " + yamlPath + ": " + e.what());
    }
}

void ResourceManager::loadTilesetFromAtlasYAML(const std::string& yamlPath, const std::string& imagePath, const std::string& name) {
    LOG_INFO("Loading tileset from atlas YAML: {} (image: {})", yamlPath, imagePath);

    try {

        std::filesystem::path projectRoot = findProjectRoot();
        std::filesystem::path yamlPathObj(yamlPath);
        std::filesystem::path imagePathObj(imagePath);


        if (yamlPathObj.is_relative()) {
            yamlPathObj = projectRoot / yamlPathObj;
            if (!std::filesystem::exists(yamlPathObj)) {
                yamlPathObj = std::filesystem::absolute(yamlPath);
            }
        }


        if (imagePathObj.is_relative()) {
            std::filesystem::path yamlDir = yamlPathObj.parent_path();
            std::filesystem::path fullImagePath = yamlDir / imagePathObj;
            if (!std::filesystem::exists(fullImagePath)) {
                fullImagePath = projectRoot / imagePathObj;
            }
            imagePathObj = fullImagePath;
        }

        if (!std::filesystem::exists(yamlPathObj)) {
            throw std::runtime_error("YAML file does not exist: " + yamlPathObj.string());
        }
        if (!std::filesystem::exists(imagePathObj)) {
            throw std::runtime_error("Image file does not exist: " + imagePathObj.string());
        }

        LOG_DEBUG("Loading atlas YAML from: {} (image: {})", yamlPathObj.string(), imagePathObj.string());
        YAML::Node config = YAML::LoadFile(yamlPathObj.string());

        if (!config["tiles"]) {
            throw std::runtime_error("YAML file missing 'tiles' root node");
        }


        int tileSize = 16;
        if (config["meta"] && config["meta"]["tile_size"]) {
            tileSize = config["meta"]["tile_size"].as<int>();
        }


        auto spriteSheet = std::make_unique<SpriteSheet>(m_renderer, imagePathObj.string());


        YAML::Node tilesNode = config["tiles"];
        int tileCount = 0;

        for (const auto& tileNode : tilesNode) {
            if (!tileNode["id"]) {
                continue;
            }

            std::string tileId = tileNode["id"].as<std::string>();
            int atlasX = tileNode["atlas_x"] ? tileNode["atlas_x"].as<int>() : 0;
            int atlasY = tileNode["atlas_y"] ? tileNode["atlas_y"].as<int>() : 0;
            int tileSizeOverride = tileNode["tile_size"] ? tileNode["tile_size"].as<int>() : tileSize;


            spriteSheet->addSprite(tileId, atlasX, atlasY, tileSizeOverride, tileSizeOverride);
            tileCount++;
        }


        m_spriteSheets[name] = std::move(spriteSheet);
        LOG_INFO("Loaded tileset '{}' with {} tiles from atlas coordinates", name, tileCount);

    } catch (const YAML::Exception& e) {
        throw std::runtime_error("YAML parsing error in " + yamlPath + ": " + e.what());
    } catch (const std::exception& e) {
        throw std::runtime_error("Error loading tileset from " + yamlPath + ": " + e.what());
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
    LOG_INFO("ResourceManager cleared");
}

}
