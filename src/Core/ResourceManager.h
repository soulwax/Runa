// File: src/Core/ResourceManager.h

#ifndef RUNA_CORE_RESOURCEMANAGER_H
#define RUNA_CORE_RESOURCEMANAGER_H

#include "RunaAPI.h"
#include "Graphics/SpriteSheet.h"
#include "Graphics/Renderer.h"
#include <string>
#include <unordered_map>
#include <memory>

namespace Runa {

class RUNA_API ResourceManager {
public:
    explicit ResourceManager(Renderer& renderer);
    ~ResourceManager() = default;


    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;


    void loadSpriteSheetFromYAML(const std::string& yamlPath);





    void loadTilesetFromAtlasYAML(const std::string& yamlPath, const std::string& imagePath, const std::string& name);


    SpriteSheet* getSpriteSheet(const std::string& name);
    const SpriteSheet* getSpriteSheet(const std::string& name) const;


    const Sprite* getSprite(const std::string& sheetName, const std::string& spriteName) const;


    std::vector<std::string> getSpriteSheetNames() const;


    void clear();

private:
    Renderer& m_renderer;
    std::unordered_map<std::string, std::unique_ptr<SpriteSheet>> m_spriteSheets;

    void parseYAML(const std::string& yamlPath);
};

}

#endif
