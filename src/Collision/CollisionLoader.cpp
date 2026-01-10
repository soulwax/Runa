#include "CollisionLoader.h"
#include "../Core/Log.h"
#include <yaml-cpp/yaml.h>
#include <fstream>
#include <algorithm>
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

namespace Runa {

int CollisionLoader::loadFromYAML(const std::string& filePath, CollisionMap& collisionMap,
                                   SpriteSheet* spriteSheet) {
    try {
        YAML::Node root = YAML::LoadFile(filePath);
        
        if (!root["tiles"]) {
            LOG_WARN("CollisionLoader: No 'tiles' section in {}", filePath);
            return 0;
        }
        
        int tileSize = 16;
        if (root["meta"] && root["meta"]["tile_size"]) {
            tileSize = root["meta"]["tile_size"].as<int>();
        }
        
        int count = 0;
        
        for (const auto& tileNode : root["tiles"]) {
            TileDefinition def;
            
            // Name - support both 'name' and 'type' fields
            if (tileNode["name"]) {
                def.name = tileNode["name"].as<std::string>();
            } else if (tileNode["type"]) {
                def.name = tileNode["type"].as<std::string>();
            } else if (tileNode["id"]) {
                def.name = "tile_" + std::to_string(tileNode["id"].as<int>());
            }
            
            // Collision type - support multiple formats
            if (tileNode["collision"]) {
                // New format: collision: solid/none/liquid/etc
                def.collision = parseCollisionType(tileNode["collision"].as<std::string>());
            } else if (tileNode["has_collision"]) {
                // Existing format: has_collision + blocks_movement
                bool hasCollision = tileNode["has_collision"].as<bool>();
                bool blocksMovement = tileNode["blocks_movement"] ? 
                    tileNode["blocks_movement"].as<bool>() : hasCollision;
                
                if (blocksMovement) {
                    def.collision = CollisionType::Solid;
                } else if (hasCollision) {
                    def.collision = CollisionType::Trigger;
                } else {
                    def.collision = CollisionType::None;
                }
            } else if (tileNode["walkable"]) {
                // Fallback: use walkable flag
                def.collision = tileNode["walkable"].as<bool>() ? 
                    CollisionType::None : CollisionType::Solid;
            }
            
            // Pixel-perfect collision mask
            bool usePixelCollision = false;
            if (tileNode["pixel_collision"]) {
                usePixelCollision = tileNode["pixel_collision"].as<bool>();
            } else if (tileNode["contains_transparency"]) {
                // If tile has transparency, it might benefit from pixel collision
                usePixelCollision = tileNode["contains_transparency"].as<bool>();
            }
            
            if (usePixelCollision && def.collision != CollisionType::None) {
                int atlasX = tileNode["atlas_x"] ? tileNode["atlas_x"].as<int>() : 0;
                int atlasY = tileNode["atlas_y"] ? tileNode["atlas_y"].as<int>() : 0;
                int width = tileNode["width"] ? tileNode["width"].as<int>() : 
                           (tileNode["tile_size"] ? tileNode["tile_size"].as<int>() : tileSize);
                int height = tileNode["height"] ? tileNode["height"].as<int>() : 
                            (tileNode["tile_size"] ? tileNode["tile_size"].as<int>() : tileSize);
                
                if (spriteSheet) {
                    def.pixelMask = createMaskFromSprite(spriteSheet, atlasX, atlasY, width, height);
                } else {
                    def.pixelMask = std::make_shared<CollisionMask>(CollisionMask::solid(width, height));
                }
            }
            
            // Interaction - support both new and existing formats
            if (tileNode["interaction"]) {
                const auto& interNode = tileNode["interaction"];
                
                if (interNode["type"]) {
                    def.interaction.type = parseInteractionType(interNode["type"].as<std::string>());
                }
                
                // Type-specific data
                if (interNode["message"]) {
                    def.interaction.data = interNode["message"].as<std::string>();
                } else if (interNode["loot_table"]) {
                    def.interaction.data = interNode["loot_table"].as<std::string>();
                } else if (interNode["data"]) {
                    def.interaction.data = interNode["data"].as<std::string>();
                }
                
                // Teleport data
                if (interNode["target_scene"]) {
                    def.interaction.targetScene = interNode["target_scene"].as<std::string>();
                }
                if (interNode["target_x"]) {
                    def.interaction.targetX = interNode["target_x"].as<float>();
                }
                if (interNode["target_y"]) {
                    def.interaction.targetY = interNode["target_y"].as<float>();
                }
                
                // One-time flag
                if (interNode["one_time"]) {
                    def.interaction.oneTime = interNode["one_time"].as<bool>();
                }
            } else if (tileNode["interactable"] && tileNode["interactable"].as<bool>()) {
                // Existing format: just marks as interactable
                def.interaction.type = InteractionType::Toggle;
            }
            
            collisionMap.addTileDefinition(def);
            count++;
        }
        
        LOG_INFO("CollisionLoader: Loaded {} tile definitions from {}", count, filePath);
        return count;
        
    } catch (const YAML::Exception& e) {
        LOG_ERROR("CollisionLoader: Failed to load {}: {}", filePath, e.what());
        return 0;
    }
}

CollisionType CollisionLoader::parseCollisionType(const std::string& str) {
    std::string lower = str;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    
    if (lower == "none" || lower == "passable") {
        return CollisionType::None;
    } else if (lower == "solid" || lower == "block") {
        return CollisionType::Solid;
    } else if (lower == "liquid" || lower == "water") {
        return CollisionType::Liquid;
    } else if (lower == "platform" || lower == "oneway") {
        return CollisionType::Platform;
    } else if (lower == "trigger") {
        return CollisionType::Trigger;
    } else if (lower == "hazard" || lower == "damage") {
        return CollisionType::Hazard;
    }
    
    LOG_WARN("CollisionLoader: Unknown collision type '{}', defaulting to None", str);
    return CollisionType::None;
}

InteractionType CollisionLoader::parseInteractionType(const std::string& str) {
    std::string lower = str;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    
    if (lower == "none") {
        return InteractionType::None;
    } else if (lower == "read" || lower == "sign" || lower == "text") {
        return InteractionType::Read;
    } else if (lower == "container" || lower == "chest" || lower == "loot") {
        return InteractionType::Container;
    } else if (lower == "teleport" || lower == "door" || lower == "portal" || lower == "warp") {
        return InteractionType::Teleport;
    } else if (lower == "toggle" || lower == "switch" || lower == "lever") {
        return InteractionType::Toggle;
    } else if (lower == "pickup" || lower == "item" || lower == "collect") {
        return InteractionType::Pickup;
    } else if (lower == "talk" || lower == "npc" || lower == "dialogue") {
        return InteractionType::Talk;
    }
    
    LOG_WARN("CollisionLoader: Unknown interaction type '{}', defaulting to None", str);
    return InteractionType::None;
}

std::shared_ptr<CollisionMask> CollisionLoader::createMaskFromSprite(
    SpriteSheet* spriteSheet, int atlasX, int atlasY, 
    int width, int height, uint8_t alphaThreshold) {
    
    if (!spriteSheet) {
        LOG_WARN("CollisionLoader: No sprite sheet provided for pixel mask");
        return std::make_shared<CollisionMask>(CollisionMask::solid(width, height));
    }
    
    // Load the image file directly with SDL_image to extract pixel data
    // (VK2D textures are GPU resources and can't be read back easily)
    std::string imagePath = spriteSheet->getTexturePath();
    
    SDL_Surface* imageSurface = IMG_Load(imagePath.c_str());
    if (!imageSurface) {
        LOG_WARN("CollisionLoader: Failed to load image '{}' for pixel mask: {}", 
                 imagePath, SDL_GetError());
        return std::make_shared<CollisionMask>(CollisionMask::solid(width, height));
    }
    
    // Convert to RGBA format (SDL3 surfaces may be in various formats)
    SDL_Surface* rgbaSurface = SDL_ConvertSurface(imageSurface, SDL_PIXELFORMAT_RGBA32);
    SDL_DestroySurface(imageSurface);
    if (!rgbaSurface) {
        LOG_WARN("CollisionLoader: Failed to convert image to RGBA format: {}", SDL_GetError());
        return std::make_shared<CollisionMask>(CollisionMask::solid(width, height));
    }
    imageSurface = rgbaSurface;
    
    // Validate sprite region bounds
    if (atlasX + width > imageSurface->w || atlasY + height > imageSurface->h) {
        LOG_WARN("CollisionLoader: Sprite region ({}, {}, {}x{}) exceeds image bounds ({}x{})",
                 atlasX, atlasY, width, height, imageSurface->w, imageSurface->h);
        SDL_DestroySurface(imageSurface);
        return std::make_shared<CollisionMask>(CollisionMask::solid(width, height));
    }
    
    // Extract the sprite region
    // Create a new surface for just this sprite
    SDL_Surface* spriteSurface = SDL_CreateSurface(width, height, SDL_PIXELFORMAT_RGBA32);
    if (!spriteSurface) {
        LOG_WARN("CollisionLoader: Failed to create sprite surface");
        SDL_DestroySurface(imageSurface);
        return std::make_shared<CollisionMask>(CollisionMask::solid(width, height));
    }
    
    // Copy the sprite region from the image
    SDL_Rect srcRect = {atlasX, atlasY, width, height};
    SDL_Rect dstRect = {0, 0, width, height};
    if (SDL_BlitSurface(imageSurface, &srcRect, spriteSurface, &dstRect) != 0) {
        LOG_WARN("CollisionLoader: Failed to extract sprite region: {}", SDL_GetError());
        SDL_DestroySurface(spriteSurface);
        SDL_DestroySurface(imageSurface);
        return std::make_shared<CollisionMask>(CollisionMask::solid(width, height));
    }
    
    // Create collision mask from alpha channel
    const uint8_t* pixels = static_cast<const uint8_t*>(spriteSurface->pixels);
    int stride = spriteSurface->pitch;  // Bytes per row
    
    auto mask = std::make_shared<CollisionMask>(
        CollisionMask::fromAlphaChannel(pixels, width, height, stride, alphaThreshold));
    
    // Count solid vs transparent pixels for debugging
    int solidPixels = 0;
    int transparentPixels = 0;
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            const uint8_t* pixel = pixels + (y * stride) + (x * 4);
            uint8_t alpha = pixel[3];
            if (alpha >= alphaThreshold) {
                solidPixels++;
            } else {
                transparentPixels++;
            }
        }
    }
    
    // Clean up
    SDL_DestroySurface(spriteSurface);
    SDL_DestroySurface(imageSurface);
    
    LOG_INFO("CollisionLoader: Created pixel-perfect mask for sprite at ({}, {}) size {}x{} - {} solid, {} transparent", 
              atlasX, atlasY, width, height, solidPixels, transparentPixels);
    
    return mask;
}

} // namespace Runa
