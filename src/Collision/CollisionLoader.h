#ifndef RUNA_COLLISION_COLLISIONLOADER_H
#define RUNA_COLLISION_COLLISIONLOADER_H

#include "../RunaAPI.h"
#include "CollisionMap.h"
#include "../runapch.h"
#include "../Graphics/SpriteSheet.h"
#include <string>
#include <memory>

namespace Runa {

/**
 * Loads collision and interaction data from YAML sprite sheet definitions.
 * 
 * Expected YAML format in sprite sheets:
 * 
 * tiles:
 * - id: 0
 *   name: "grass"
 *   atlas_x: 0
 *   atlas_y: 0
 *   collision: none          # none, solid, liquid, platform, trigger, hazard
 *   
 * - id: 1
 *   name: "fence"
 *   atlas_x: 16
 *   atlas_y: 0
 *   collision: solid
 *   pixel_collision: true    # Use alpha channel for pixel-perfect collision
 *   
 * - id: 2
 *   name: "sign"
 *   atlas_x: 32
 *   atlas_y: 0
 *   collision: solid
 *   interaction:
 *     type: read             # read, container, teleport, toggle, pickup, talk
 *     message: "Welcome!"
 *     
 * - id: 3
 *   name: "door"
 *   atlas_x: 48
 *   atlas_y: 0
 *   collision: solid
 *   interaction:
 *     type: teleport
 *     target_scene: "house_interior"
 *     target_x: 5.0
 *     target_y: 10.0
 *     
 * - id: 4
 *   name: "chest"
 *   atlas_x: 64
 *   atlas_y: 0
 *   collision: solid
 *   interaction:
 *     type: container
 *     loot_table: "common_chest"
 *     one_time: true
 */
class RUNA_API CollisionLoader {
public:
    /**
     * Load tile definitions from a YAML file into a CollisionMap.
     * @param filePath Path to the YAML sprite sheet file
     * @param collisionMap CollisionMap to add definitions to
     * @param spriteSheet Optional SpriteSheet to extract pixel masks from
     * @return Number of tile definitions loaded
     */
    static int loadFromYAML(const std::string& filePath, CollisionMap& collisionMap,
                            SpriteSheet* spriteSheet = nullptr);
    
    /**
     * Parse collision type from string
     */
    static CollisionType parseCollisionType(const std::string& str);
    
    /**
     * Parse interaction type from string
     */
    static InteractionType parseInteractionType(const std::string& str);
    
    /**
     * Create a pixel-perfect collision mask from a sprite's alpha channel.
     * Requires the texture to be loadable/readable.
     * @param spriteSheet The sprite sheet containing the tile
     * @param atlasX X position in atlas
     * @param atlasY Y position in atlas
     * @param width Tile width
     * @param height Tile height
     * @param alphaThreshold Alpha value threshold (0-255) for solid pixels
     */
    static std::shared_ptr<CollisionMask> createMaskFromSprite(
        SpriteSheet* spriteSheet, int atlasX, int atlasY, 
        int width, int height, uint8_t alphaThreshold = 128);
};

} // namespace Runa

#endif // RUNA_COLLISION_COLLISIONLOADER_H
