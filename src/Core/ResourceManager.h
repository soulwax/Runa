// File: src/Core/ResourceManager.h

#pragma once

#include "../RunaAPI.h"
#include "../Graphics/SpriteSheet.h"
#include "../Graphics/Renderer.h"
#include <string>
#include <unordered_map>
#include <memory>

namespace Runa {

class RUNA_API ResourceManager {
public:
    explicit ResourceManager(Renderer& renderer);
    ~ResourceManager() = default;

    // Disable copying
    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;

    // Load spritesheet from YAML manifest
    void loadSpriteSheetFromYAML(const std::string& yamlPath);

    // Get spritesheet by name
    SpriteSheet* getSpriteSheet(const std::string& name);
    const SpriteSheet* getSpriteSheet(const std::string& name) const;

    // Get sprite from any loaded spritesheet
    const Sprite* getSprite(const std::string& sheetName, const std::string& spriteName) const;

    // List all loaded spritesheets
    std::vector<std::string> getSpriteSheetNames() const;

    // Clear all resources
    void clear();

private:
    Renderer& m_renderer;
    std::unordered_map<std::string, std::unique_ptr<SpriteSheet>> m_spriteSheets;

    void parseYAML(const std::string& yamlPath);
};

} // namespace Runa
